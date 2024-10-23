// Main visualization script
document.addEventListener('DOMContentLoaded', function() {
    // Load the CSV data
    d3.csv('../Data/profile_stats.csv').then(data => {
        // Parse numeric values
        data.forEach(d => {
            d['Call Count'] = +d['Call Count'];
            d['Total Time'] = +d['Total Time'];
            d['Min Time'] = +d['Min Time'];
            d['Max Time'] = +d['Max Time'];
            d['Avg Time'] = +d['Avg Time'];
        });

        // Initialize visualizations
        initializeFilters(data);
        createBarChart(data);
        createFlameGraph(data);
        createTimeline(data);
        updateTable(data);

        // Add event listeners for controls
        document.getElementById('sortSelect').addEventListener('change', () => updateVisualizations(data));
        document.getElementById('functionFilter').addEventListener('change', () => updateVisualizations(data));
        document.getElementById('viewSelect').addEventListener('change', () => updateVisualizations(data));
    });
});

function initializeFilters(data) {
    // Populate function filter dropdown
    const functionFilter = document.getElementById('functionFilter');
    const functions = [...new Set(data.map(d => d['Function Name']))];
    functions.forEach(func => {
        const option = document.createElement('option');
        option.value = func;
        option.textContent = func;
        functionFilter.appendChild(option);
    });
}

function createBarChart(data) {
    const margin = {top: 20, right: 20, bottom: 40, left: 60};
    const width = document.getElementById('barChart').clientWidth - margin.left - margin.right;
    const height = document.getElementById('barChart').clientHeight - margin.top - margin.bottom;

    // Clear existing content
    d3.select('#barChart').html('');

    const svg = d3.select('#barChart')
        .append('svg')
        .attr('width', width + margin.left + margin.right)
        .attr('height', height + margin.top + margin.bottom)
        .append('g')
        .attr('transform', `translate(${margin.left},${margin.top})`);

    // Create scales
    const x = d3.scaleBand()
        .range([0, width])
        .padding(0.1);

    const y = d3.scaleLinear()
        .range([height, 0]);

    // Set domains
    x.domain(data.map(d => d['Section Name']));
    y.domain([0, d3.max(data, d => d['Total Time'])]);

    // Add bars
    svg.selectAll('.bar')
        .data(data)
        .enter()
        .append('rect')
        .attr('class', 'bar')
        .attr('x', d => x(d['Section Name']))
        .attr('width', x.bandwidth())
        .attr('y', d => y(d['Total Time']))
        .attr('height', d => height - y(d['Total Time']))
        .on('mouseover', function(event, d) {
            showTooltip(event, d);
        })
        .on('mouseout', hideTooltip);

    // Add axes
    svg.append('g')
        .attr('transform', `translate(0,${height})`)
        .call(d3.axisBottom(x))
        .selectAll('text')
        .attr('transform', 'rotate(-45)')
        .style('text-anchor', 'end');

    svg.append('g')
        .call(d3.axisLeft(y));

    // Add labels
    svg.append('text')
        .attr('class', 'axis-label')
        .attr('transform', 'rotate(-90)')
        .attr('y', 0 - margin.left)
        .attr('x', 0 - (height / 2))
        .attr('dy', '1em')
        .style('text-anchor', 'middle')
        .text('Total Time (seconds)');
}

function createFlameGraph(data) {
    // Organize data into hierarchy
    const root = d3.stratify()
        .id(d => d['Section Name'])
        .parentId(d => getParentSection(d['Section Name']))
        (data);

    const margin = {top: 20, right: 20, bottom: 20, left: 20};
    const width = document.getElementById('flameGraph').clientWidth - margin.left - margin.right;
    const height = document.getElementById('flameGraph').clientHeight - margin.top - margin.bottom;

    // Clear existing content
    d3.select('#flameGraph').html('');

    const svg = d3.select('#flameGraph')
        .append('svg')
        .attr('width', width + margin.left + margin.right)
        .attr('height', height + margin.top + margin.bottom)
        .append('g')
        .attr('transform', `translate(${margin.left},${margin.top})`);

    // Create partition layout
    const partition = d3.partition()
        .size([width, height])
        .padding(1);

    root.sum(d => d['Total Time']);
    partition(root);

    // Add rectangles
    svg.selectAll('rect')
        .data(root.descendants())
        .enter()
        .append('rect')
        .attr('class', 'flame-node')
        .attr('x', d => d.x0)
        .attr('y', d => d.y0)
        .attr('width', d => d.x1 - d.x0)
        .attr('height', d => d.y1 - d.y0)
        .on('mouseover', function(event, d) {
            showTooltip(event, d.data);
        })
        .on('mouseout', hideTooltip);
}

function createTimeline(data) {
    const margin = {top: 20, right: 20, bottom: 40, left: 60};
    const width = document.getElementById('timelineChart').clientWidth - margin.left - margin.right;
    const height = document.getElementById('timelineChart').clientHeight - margin.top - margin.bottom;

    // Clear existing content
    d3.select('#timelineChart').html('');

    const svg = d3.select('#timelineChart')
        .append('svg')
        .attr('width', width + margin.left + margin.right)
        .attr('height', height + margin.top + margin.bottom)
        .append('g')
        .attr('transform', `translate(${margin.left},${margin.top})`);

    // Create scales
    const x = d3.scaleLinear()
        .domain([0, d3.max(data, d => d['Total Time'])])
        .range([0, width]);

    const y = d3.scaleBand()
        .domain(data.map(d => d['Section Name']))
        .range([0, height])
        .padding(0.1);

    // Add timeline bars
    svg.selectAll('.timeline-bar')
        .data(data)
        .enter()
        .append('rect')
        .attr('class', 'timeline-bar')
        .attr('x', 0)
        .attr('y', d => y(d['Section Name']))
        .attr('width', d => x(d['Total Time']))
        .attr('height', y.bandwidth())
        .on('mouseover', function(event, d) {
            showTooltip(event, d);
        })
        .on('mouseout', hideTooltip);

    // Add axes
    svg.append('g')
        .attr('transform', `translate(0,${height})`)
        .call(d3.axisBottom(x));

    svg.append('g')
        .call(d3.axisLeft(y));

    // Add label
    svg.append('text')
        .attr('class', 'axis-label')
        .attr('x', width / 2)
        .attr('y', height + margin.bottom - 5)
        .style('text-anchor', 'middle')
        .text('Time (seconds)');
}

function updateTable(data) {
    const tbody = document.querySelector('#statsTable tbody');
    tbody.innerHTML = '';

    data.forEach(d => {
        const row = document.createElement('tr');
        row.className = 'table-row-hover';
        row.innerHTML = `
            <td class="px-6 py-4 whitespace-nowrap">${d['Section Name']}</td>
            <td class="px-6 py-4 whitespace-nowrap">${d['Call Count']}</td>
            <td class="px-6 py-4 whitespace-nowrap">${d['Total Time'].toFixed(6)}</td>
            <td class="px-6 py-4 whitespace-nowrap">${d['Avg Time'].toFixed(6)}</td>
            <td class="px-6 py-4 whitespace-nowrap">${d['Function Name']}</td>
        `;
        tbody.appendChild(row);
    });
}

function updateVisualizations(data) {
    // Get current filter and sort values
    const sortBy = document.getElementById('sortSelect').value;
    const functionFilter = document.getElementById('functionFilter').value;
    const viewType = document.getElementById('viewSelect').value;

    // Filter data
    let filteredData = data;
    if (functionFilter !== 'all') {
        filteredData = data.filter(d => d['Function Name'] === functionFilter);
    }

    // Sort data
    filteredData.sort((a, b) => {
        switch (sortBy) {
            case 'time':
                return b['Total Time'] - a['Total Time'];
            case 'calls':
                return b['Call Count'] - a['Call Count'];
            default:
                return a['Section Name'].localeCompare(b['Section Name']);
        }
    });

    // Update visualizations based on view type
    if (viewType === 'performance') {
        createBarChart(filteredData);
        createTimeline(filteredData);
    } else {
        createFlameGraph(filteredData);
    }

    // Always update the table
    updateTable(filteredData);
}

function getParentSection(sectionName) {
    // Extract parent section name based on naming convention
    const parts = sectionName.split(' - ');
    if (parts.length > 1) {
        return parts[0];
    }
    return null;
}

function showTooltip(event, d) {
    const tooltip = d3.select('body').append('div')
        .attr('class', 'tooltip')
        .style('left', (event.pageX + 10) + 'px')
        .style('top', (event.pageY - 10) + 'px');

    tooltip.html(`
        <strong>${d['Section Name']}</strong><br/>
        Calls: ${d['Call Count']}<br/>
        Total Time: ${d['Total Time'].toFixed(6)}s<br/>
        Avg Time: ${d['Avg Time'].toFixed(6)}s<br/>
        Min Time: ${d['Min Time'].toFixed(6)}s<br/>
        Max Time: ${d['Max Time'].toFixed(6)}s
    `);
}

function hideTooltip() {
    d3.selectAll('.tooltip').remove();
}

// Add window resize handler
window.addEventListener('resize', debounce(() => {
    const data = d3.select('#barChart').datum();
    if (data) {
        updateVisualizations(data);
    }
}, 250));

// Utility function for debouncing resize events
function debounce(func, wait) {
    let timeout;
    return function executedFunction(...args) {
        const later = () => {
            clearTimeout(timeout);
            func(...args);
        };
        clearTimeout(timeout);
        timeout = setTimeout(later, wait);
    };
}

// Add error handling
function handleError(error) {
    console.error('Error in profiler dashboard:', error);
    // Display error message to user
    const errorDiv = document.createElement('div');
    errorDiv.className = 'bg-red-100 border border-red-400 text-red-700 px-4 py-3 rounded relative';
    errorDiv.role = 'alert';
    errorDiv.innerHTML = `
        <strong class="font-bold">Error!</strong>
        <span class="block sm:inline">Failed to load or process profiler data.</span>
    `;
    document.querySelector('.container').prepend(errorDiv);
}

// Initialize responsive behavior
function initializeResponsiveness() {
    const resizeObserver = new ResizeObserver(debounce(() => {
        const data = d3.select('#barChart').datum();
        if (data) {
            updateVisualizations(data);
        }
    }, 250));

    // Observe size changes on chart containers
    ['barChart', 'flameGraph', 'timelineChart'].forEach(id => {
        const element = document.getElementById(id);
        if (element) {
            resizeObserver.observe(element);
        }
    });
}

// Call initialization when DOM is loaded
document.addEventListener('DOMContentLoaded', function() {
    try {
        // Load the CSV data
        d3.csv('../Data/profile_stats.csv').then(data => {
            // Store the original data
            d3.select('#barChart').datum(data);
            
            // Initialize everything
            initializeFilters(data);
            updateVisualizations(data);
            initializeResponsiveness();
        }).catch(handleError);
    } catch (error) {
        handleError(error);
    }
});

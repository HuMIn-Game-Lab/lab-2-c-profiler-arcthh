// Global variables for holding the data and chart instances
let profilerData = [];
let overviewChart = null;
let detailChart = null;
let currentMetric = 'Total Time';

// Function to format numbers in a human-readable way
function formatNumber(num) {
    return num < 1 ? num.toFixed(6) : num.toFixed(2);
}

// Function to create the overview chart
function createOverviewChart(data) {
    if (overviewChart) overviewChart.destroy();

    const ctx = document.getElementById('overviewChart').getContext('2d');
    const labels = data.map(row => row['Section Name']);
    const totalTimes = data.map(row => row['Total Time']);
    
    overviewChart = new Chart(ctx, {
        type: 'bar',
        data: {
            labels: labels,
            datasets: [{
                label: 'Total Time (ms)',
                data: totalTimes,
                backgroundColor: 'rgba(54, 162, 235, 0.8)',
                borderColor: 'rgba(54, 162, 235, 1)',
                borderWidth: 1
            }]
        },
        options: {
            scales: {
                y: {
                    beginAtZero: true
                }
            },
            plugins: {
                tooltip: {
                    callbacks: {
                        label: function(context) {
                            return `${context.dataset.label}: ${formatNumber(context.raw)}`;
                        }
                    }
                }
            }
        }
    });
}

// Function to update the detailed chart based on the selected metric
function updateDetailChart(metric) {
    currentMetric = metric;
    const labels = profilerData.map(row => row['Section Name']);
    const data = profilerData.map(row => row[metric]);

    if (detailChart) detailChart.destroy();

    const ctx = document.getElementById('detailChart').getContext('2d');
    detailChart = new Chart(ctx, {
        type: 'bar',
        data: {
            labels: labels,
            datasets: [{
                label: metric,
                data: data,
                backgroundColor: 'rgba(255, 99, 132, 0.8)',
                borderColor: 'rgba(255, 99, 132, 1)',
                borderWidth: 1
            }]
        },
        options: {
            scales: {
                y: {
                    beginAtZero: true
                }
            },
            plugins: {
                tooltip: {
                    callbacks: {
                        label: function(context) {
                            return `${context.dataset.label}: ${formatNumber(context.raw)}`;
                        }
                    }
                }
            }
        }
    });

    // Update button styles
    document.querySelectorAll('.controls button').forEach(button => {
        button.classList.remove('active');
        if (button.textContent === metric) {
            button.classList.add('active');
        }
    });
}

fetch('./profile_stats.json')  // Adjusted to go up one directory to find the Data folder
    .then(response => response.json())
    .then(json => {
        profilerData = json;  // Directly use the parsed JSON data
        createOverviewChart(profilerData); // Create the overview chart
        updateDetailChart('Total Time'); // Create the detail chart
    })
    .catch(error => {
        console.error('Error loading JSON:', error);
        alert('Error loading JSON file.');
    });

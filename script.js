// Global variables
let flowChart, totalChart;
let timeLabels = [];       // Stores timestamps for chart X-axis
let flowData = [];         // Stores flow rate values (L/min)
let totalData = [];        // Stores total water values (L)
const MAX_DATA_POINTS = 20; // Limits chart history
const API_URL = "https://happykoala.infy.com/get_data.php"; // Update with your URL

// ==============================================
// Function: fetchData()
// Fetches data from PHP backend and updates UI
// ==============================================
function fetchData() {
    showLoading(true);
    
    fetch(API_URL, { 
        headers: { "Cache-Control": "no-cache" } // Prevent cached responses
    })
    .then(response => {
        if (!response.ok) throw new Error("Network error");
        return response.json();
    })
    .then(data => {
        // Validate data
        if (typeof data?.flow !== "number" || typeof data?.total !== "number") {
            throw new Error("Invalid data format");
        }

        updateUI(data);
        updateChartData(data);
        checkAlerts(data);
    })
    .catch(error => {
        console.error("Fetch error:", error);
        document.getElementById("flowRate").textContent = "Error: " + error.message;
    })
    .finally(() => showLoading(false));
}

// ==============================================
// Function: updateUI()
// Updates the dashboard display
// ==============================================
function updateUI(data) {
    document.getElementById("flowRate").textContent = data.flow.toFixed(2) + " L/min";
    document.getElementById("totalWater").textContent = data.total.toFixed(2) + " L";
    document.getElementById("lastUpdate").textContent = new Date().toLocaleTimeString();
}

// ==============================================
// Function: updateChartData()
// Manages chart data arrays
// ==============================================
function updateChartData(data) {
    const now = new Date().toLocaleTimeString();
    
    // Add new data
    timeLabels.push(now);
    flowData.push(data.flow);
    totalData.push(data.total);
    
    // Remove oldest data if exceeding limit
    if (timeLabels.length > MAX_DATA_POINTS) {
        timeLabels.shift();
        flowData.shift();
        totalData.shift();
    }
    
    // Update charts
    updateCharts();
}

// ==============================================
// Function: updateCharts()
// Refreshes Chart.js visualizations
// ==============================================
function updateCharts() {
    flowChart.data.labels = timeLabels;
    flowChart.data.datasets[0].data = flowData;
    flowChart.update();
    
    totalChart.data.labels = timeLabels;
    totalChart.data.datasets[0].data = totalData;
    totalChart.update();
}

// ==============================================
// Function: checkAlerts()
// Triggers visual alerts for abnormal values
// ==============================================
function checkAlerts(data) {
    const flowElement = document.getElementById("flowRate");
    
    // Reset previous alerts
    flowElement.classList.remove("alert");
    
    // Flow rate too high
    if (data.flow > 10) {
        flowElement.classList.add("alert");
    }
}

// ==============================================
// Function: showLoading()
// Toggles loading indicator
// ==============================================
function showLoading(show) {
    document.getElementById("loading").style.display = show ? "block" : "none";
}

// ==============================================
// Function: initializeCharts()
// Sets up Chart.js when page loads
// ==============================================
function initializeCharts() {
    const ctx1 = document.getElementById("flowChart").getContext("2d");
    const ctx2 = document.getElementById("totalChart").getContext("2d");
    
    // Flow Rate Chart
    flowChart = new Chart(ctx1, {
        type: "line",
        data: {
            labels: timeLabels,
            datasets: [{
                label: "Flow Rate (L/min)",
                backgroundColor: "rgba(0, 123, 255, 0.2)",
                borderColor: "#007bff",
                data: flowData,
                fill: true
            }]
        },
        options: {
            scales: { y: { beginAtZero: true } }
        }
    });
    
    // Total Consumption Chart
    totalChart = new Chart(ctx2, {
        type: "line",
        data: {
            labels: timeLabels,
            datasets: [{
                label: "Total Water (L)",
                backgroundColor: "rgba(255, 99, 132, 0.2)",
                borderColor: "#ff6384",
                data: totalData,
                fill: true
            }]
        }
    });
}

// ==============================================
// On Page Load: Initialize everything
// ==============================================
document.addEventListener("DOMContentLoaded", () => {
    initializeCharts();
    fetchData(); // Initial fetch
    setInterval(fetchData, 2000); // Update every 2 seconds
});
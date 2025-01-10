async function fetchData() {
    const url = "/fetch"
    try {
        const response = await fetch(url); // Adjust URL as needed
        const data = await response.json();
        msg = data['data']
        log = data['log']
        document.getElementById('data').textContent = JSON.stringify(msg, null, 2);
        document.getElementById('log').textContent = JSON.stringify(log, null, 2);
    } catch (error) {
        console.error("Error fetching data:", error);
    } finally {
        setTimeout(fetchData, 100); // Fetch data every 100ms
    }
}

document.addEventListener('DOMContentLoaded', () => {
    fetchData();
});
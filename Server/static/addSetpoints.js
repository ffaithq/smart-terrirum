function sendImageAndData() {
    const formData = new FormData();

    // Append image file from input field
    const imageFile = document.getElementById('imageFile').files[0];
    formData.append('image', imageFile);

    // Collect JSON data from input fields
    const jsonData = {
        name: document.getElementById('name').value,
        waterlevel: document.getElementById('waterlevel').value,
        uvlevel: document.getElementById('uvlevel').value
    };
    formData.append('json', JSON.stringify(jsonData));

    // Send data to Flask server
    fetch('/add_setpoint', {
        method: 'POST',
        body: formData
    })
    .then(response => response.json())
    .then(data => {
        console.log('Success:', data);
        alert('Upload successful!');
    })
    .catch(error => {
        console.error('Error:', error);
        alert('Error uploading data');
    });
}
function login() {
    const jsonData = {
        username: document.getElementById('username').value,
        password: document.getElementById('password').value,
    };

    // Send data to Flask server
    fetch('/login', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json'  // Specify content type as JSON
        },
        body: JSON.stringify(jsonData)  // Convert the JSON object to a string
    })
    .then(response => response.json())
    .then(data => {
        console.log('Success:', data);
        alert('Login successful!');
    })
    .catch(error => {
        console.error('Error:', error);
        alert('Error during registration');
    });
}

function register() {
    const jsonData = {
        username: document.getElementById('username').value,
        password: document.getElementById('password').value,
        usertype: document.getElementById('usertype').value
    };

    // Send data to Flask server
    fetch('/register', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json'  // Specify content type as JSON
        },
        body: JSON.stringify(jsonData)  // Convert the JSON object to a string
    })
    .then(response => response.json())
    .then(data => {
        console.log('Success:', data);
        alert('Registration successful!');
    })
    .catch(error => {
        console.error('Error:', error);
        alert('Error during registration');
    });
}

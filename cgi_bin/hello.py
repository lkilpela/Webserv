import cgi
import os

print("HTTP/1.1 200 OK")
print("Content-type: text/html\n")

# Handle form data
form = cgi.FieldStorage()

if os.environ["REQUEST_METHOD"] == "POST":
    name = form.getvalue("name")
elif os.environ["REQUEST_METHOD"] == "GET":
    name = form.getvalue("name", "World")

print(f"""
<!DOCTYPE html>
<html>
    <head>
        <title>Moving Gradient</title>
        <style>
            body {{
                margin: 0;
                height: 100vh;
                display: flex;
                justify-content: center;
                align-items: center;
                font-family: Arial, sans-serif;
                background: linear-gradient(45deg, #ff9a9e, #fad0c4, #fbc2eb, #a18cd1);
                background-size: 400% 400%;
                animation: gradientBackground 10s ease infinite;
            }}

            @keyframes gradientBackground {{
                0% {{
                    background-position: 0% 50%;
                }}
                50% {{
                    background-position: 100% 50%;
                }}
                100% {{
                    background-position: 0% 50%;
                }}
            }}
        </style>
    </head>
    <body>
        <h1>Hello, {name}!</h1>
    </body>
</html>
""")

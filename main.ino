#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <LittleFS.h>

// WiFi credentials
const char* ssid = "ESP32-Exam-System";
const char* password = "password";

// Server
AsyncWebServer server(80);

// Admin and Customer credentials
String adminUsername = "admin";
String adminPassword = "admin123";
String customerUsername = "customer";
String customerPassword = "customer123";

String serverIndex = "<script src='https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js'></script>"
"<form method='POST' action='#' enctype='multipart/form-data' id='upload_form'>"
    "<input type='file' name='update'>"
    "<input type='submit' value='Upload'>"
"</form>"
"<div id='prg'>progress: 0%</div>"
"<script>"
"$('form').submit(function(e){"
    "e.preventDefault();"
      "var form = $('#upload_form')[0];"
      "var data = new FormData(form);"
      " $.ajax({"
            "url: '/upload',"
            "type: 'POST',"               
            "data: data,"
            "contentType: false,"                  
            "processData:false,"  
            "xhr: function() {"
                "var xhr = new window.XMLHttpRequest();"
                "xhr.upload.addEventListener('progress', function(evt) {"
                    "if (evt.lengthComputable) {"
                        "var per = evt.loaded / evt.total;"
                        "$('#prg').html('progress: ' + Math.round(per*100) + '%');"
                    "}"
               "}, false);"
               "return xhr;"
            "},"                                
            "success:function(d, s) {"    
                "console.log('success!')"
           "},"
            "error: function (a, b, c) {"
            "}"
          "});"
"});"
"</script>";

const char* login = R"(
<!DOCTYPE html>
<html>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">

<head>
    <title>Exam Page</title>
</head>
<style>
    * {
        margin: 0;
        padding: 0;
        box-sizing: border-box;
    }

    .bg-image {
        background-color: rgb(77, 252, 182);
        /* background-image: url('./bg-image.jpg');
    background-size: cover;
    background-position: center;
    background-repeat: no-repeat; */
        height: 100vh;
        width: 100vw;
        display: flex;
        justify-content: center;
        align-items: center;
    }

    .bg-image .opacity {
        background-color: #fff;
        height: 350px;
        width: 400px;
        position: absolute;
        border-radius: 10px;
    }

    .bg-image .container {
        background-color: transparent;
        height: 350px;
        width: 400px;
        position: absolute;
        display: flex;
        justify-content: center;
        align-items: center;
        /* border: 1px darkcyan dashed; */
        border-radius: 10px;
    }

    .bg-image .container .form {
        display: flex;
        flex-direction: column;
        justify-content: space-around;
        align-items: center;
        height: 100%;
    }

    .bg-image .container .form h2 {
        font-size: 26px;
        margin-top: 20px;
    }

    .bg-image .container .form form {
        height: 140px;
        width: 300px;
        display: flex;
        flex-direction: column;
    }

    .bg-image .container .form form input {
        width: 300px;
        height: 30px;
        border-radius: 6px;
        border: none;
        border: 1px darkcyan dashed;
    }

    .bg-image .container .form form button {
        width: 300px;
        height: 30px;
        border-radius: 6px;
        border: none;
        border: 1px darkcyan dashed;
        background-color: rgb(77, 252, 182);
        transition: all 0.5s;
    }

    .bg-image .container .form form button:hover {
        background-color: rgb(16, 111, 73);
        color: #ffF;
        transition: all 0.5s;
    }

    @media (max-width:760px) {
        .bg-image .opacity {
            height: 350px;
            width: 320px;
        }

        .bg-image .container .form h2 {
            font-size: 20px;
            margin-top: 20px
        }
    }
</style>

<body>
    <div class="bg-image">
        <div class="opacity"></div>
        <div class="container">
            <div class="form">
                <h2>Цахим шалгалтын систем</h2>
                <form action="/login" method="GET">
                    <input type="text" name="username" placeholder="Username" required><br>
                    <input type="password" name="password" placeholder="Password" required><br>
                    <button type="submit">Нэвтрэх</button>
                </form>
            </div>
        </div>
    </div>
</body>

</html>
)";

String customer = R"(
<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>customer</title>
</head>
<style>
    * {
        margin: 0;
        padding: 0;
        box-sizing: border-box;
    }

    body {
        background-color: rgb(77, 252, 182);
        display: flex;
        flex-direction: row;
        justify-content: center;
        align-items: center;
        height: 100vh;
    }

    .container {
        display: grid;
        grid-template-columns: repeat(2, 1fr);
        justify-content: center;
        align-items: center;

    }

    .container .fixed-title {
        width: 360px;
        background-color: #fff;
        border-radius: 0px 0px 10px 10px;
        border: 1px darkcyan dashed;
        display: none;
    }

    .container .left-section {
        width: 38vw;
        background-color: #fff;
        height: 70vh;
        border: 1px darkcyan dashed;
        top: 4px;
    }

    .container .left-section .exam-pdf {
        height: 70vh;
    }

    .container .left-section .exam-pdf iframe {
        width: 100%;
        height: 100%;
    }

    .container .right-section {
        width: 38vw;
        border: 1px darkcyan dashed;
    }

    .container .right-section .questions {
        max-height: 70vh;
        overflow-y: scroll;

    }

    .container .right-section .questions form {
        display: flex;
        flex-direction: column;
        justify-content: center;
        align-items: center;
        background-color: aqua;
        margin-bottom: 10px;
        position: relative;
        margin-bottom: 100px;
    }

    .container .right-section .questions form .footer {
        position: fixed;
        bottom: 112px;
        background-color: #fff;
        width: 360px;
        height: 82px;
        border-radius: 6px;
        display: flex;
        flex-direction: column;
        justify-content: center;
        align-items: center;
        gap: 8px;
    }

    .container .right-section .questions form .footer .last-input {
        text-align: center;
    }

    .container .right-section .questions form .footer .last-input input {
        width: 300px;
        border: 1px darkcyan dashed;
        padding: 3px;
    }

    .container .right-section .questions form .footer .last-input p {
        margin-bottom: 4px;
    }

    .container .right-section .questions form .footer .btn button {
        width: 300px;
        border: 1px darkcyan dashed;
        font-size: 12px;
        padding: 3px;
        background-color: rgb(77, 252, 182);
        transition: all 0.5s;
    }

    .container .right-section .questions form .footer .btn button:hover {
        background-color: rgb(16, 111, 73);
        transition: all 0.5s;
        color: #fff;
    }

    .container .right-section .questions form .form-container {
        width: 360px;
        display: grid;
        grid-template-columns: repeat(2, 1fr);
        justify-content: center;
        align-items: center;
    }

    .container .right-section .questions form .form-container .options {
        text-align: center;
        padding: 14px 2px;
    }

    .container .right-section .questions form .form-container .options label {
        padding: 0px 4px;
        margin-top: 3px;
    }

    .container .right-section .questions form .form-container .options h3 {
        margin-bottom: 4px;
    }

    /* ----------------------- mobile --------------------- */
    @media screen and (max-width:460px) {
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }

        .container {
            display: flex;
            flex-direction: column;
            justify-content: center;
            align-items: center;

        }

        .container .fixed-title {
            width: 360px;
            position: fixed;
            top: 300px;
            background-color: #fff;
            border-radius: 0px 0px 10px 10px;
            border: 1px darkcyan dashed;
            display: block;
        }

        .container .left-section .exam-pdf iframe {
            width: 100%;
            height: 294px;
        }

        .container .left-section {
            width: 360px;
            background-color: #fff;
            height: 300px;
            border: 1px darkcyan dashed;
            position: fixed;
            top: 4px;
        }

        .container .right-section {
            margin-top: 350px;
            height: 504px;
            width: 360px;
        }

        .container .right-section .questions {
            max-height: 400px;
            overflow-y: scroll;
        }

        .container .right-section .questions form {
            display: flex;
            flex-direction: column;
            justify-content: center;
            align-items: center;
            background-color: aqua;
            margin-bottom: 10px;

        }

        .container .right-section .questions form .footer {
            background-color: #fff;
            position: fixed;
            bottom: 10px;
            width: 360px;
            height: 82px;
            border-radius: 6px;
            display: flex;
            flex-direction: column;
            justify-content: center;
            align-items: center;
            gap: 8px;
        }

        .container .right-section .questions form .footer .last-input {
            text-align: center;
        }

        .container .right-section .questions form .footer .last-input input {
            width: 300px;
            border: 1px darkcyan dashed;
            padding: 3px;
        }

        .container .right-section .questions form .footer .last-input p {
            margin-bottom: 4px;
        }

        .container .right-section .questions form .footer .btn button {
            width: 300px;
            border: 1px darkcyan dashed;
            font-size: 12px;
            padding: 3px;
            background-color: rgb(77, 252, 182);
            transition: all 0.5s;
        }

        .container .right-section .questions form .footer .btn button:hover {
            background-color: rgb(16, 111, 73);
            transition: all 0.5s;
        }

        .container .right-section .questions form .form-container {
            width: 360px;
            display: grid;
            grid-template-columns: repeat(2, 1fr);
            justify-content: center;
            align-items: center;
        }

        .container .right-section .questions form .form-container .options {
            text-align: center;
            padding: 14px 2px;
        }

        .container .right-section .questions form .form-container .options label {
            padding: 0px 4px;
            margin-top: 3px;
        }

        .container .right-section .questions form .form-container .options h3 {
            margin-bottom: 4px;
        }
    }

    /*------ tablet ----------- */

    @media screen and (min-width:461px) and (max-width:1024px) {
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }

        .container {
            display: grid;
            grid-template-columns: repeat(2, 1fr);
            grid-gap: 4em;
        }

        .container .fixed-title {
            width: 360px;
            background-color: #fff;
            border-radius: 0px 0px 10px 10px;
            border: 1px darkcyan dashed;
            display: none;
        }

        .container .left-section .exam-pdf iframe {
            width: 100%;
            height: 294px;
        }

        .container .left-section {
            width: 360px;
            background-color: #fff;
            height: 504px;
            border: 1px darkcyan dashed;
        }

        .container .right-section {

            height: 504px;
            width: 360px;
        }

        .container .right-section .questions {
            max-height: 400px;
            overflow-y: scroll;
        }

        .container .right-section .questions form {
            display: flex;
            flex-direction: column;
            justify-content: center;
            align-items: center;
            background-color: aqua;
            margin-bottom: 10px;

        }

        .container .right-section .questions form .footer {
            background-color: #fff;
            /* position: fixed; */
            /* bottom: 10px; */
            margin-bottom: 65px;
            width: 360px;
            height: 82px;
            border-radius: 6px;
            display: flex;
            flex-direction: column;
            justify-content: center;
            align-items: center;
            gap: 8px;
        }

        .container .right-section .questions form .footer .last-input {
            text-align: center;
        }

        .container .right-section .questions form .footer .last-input input {
            width: 300px;
            border: 1px darkcyan dashed;
            padding: 3px;
        }

        .container .right-section .questions form .footer .last-input p {
            margin-bottom: 4px;
        }

        .container .right-section .questions form .footer .btn button {
            width: 300px;
            border: 1px darkcyan dashed;
            font-size: 12px;
            padding: 3px;
            background-color: rgb(77, 252, 182);
            transition: all 0.5s;
        }

        .container .right-section .questions form .footer .btn button:hover {
            background-color: rgb(16, 111, 73);
            transition: all 0.5s;
        }

        .container .right-section .questions form .form-container {
            width: 360px;
            display: grid;
            grid-template-columns: repeat(2, 1fr);
            justify-content: center;
            align-items: center;
        }

        .container .right-section .questions form .form-container .options {
            text-align: center;
            padding: 14px 2px;
        }

        .container .right-section .questions form .form-container .options label {
            padding: 0px 4px;
            margin-top: 3px;
        }

        .container .right-section .questions form .form-container .options h3 {
            margin-bottom: 4px;
        }
    }
</style>

<body>
    <div class="container">
        <div class="left-section">
            <div class="exam-pdf">
                <iframe src="example.pdf" frameborder="0" type="application/pdf"></iframe>
            </div>
        </div>
        <div class="fixed-title">
            <h1>Хариултаа илгээх хэсэг</h1>
        </div>

        <div class="right-section">
            <div class="questions">
                <form action="/customer" method="GET">
                    <div class="form-container">
                        <div class="options">
                            <h3>Асуулт 1</h3>
                            <label><input required type="radio" name="q1" value="A">
                                A</label>
                            <label><input required type="radio" name="q1" value="B">
                                B</label>
                            <label><input required type="radio" name="q1" value="C">
                                C</label>
                            <label><input required type="radio" name="q1" value="D">
                                D</label>
                        </div>
                        <div class="options">
                            <h3>Асуулт 2</h3>
                            <label><input required type="radio" name="q2" value="A">
                                A</label>
                            <label><input required type="radio" name="q2" value="B">
                                B</label>
                            <label><input required type="radio" name="q2" value="C">
                                C</label>
                            <label><input required type="radio" name="q2" value="D">
                                D</label>
                        </div>
                        <div class="options">
                            <h3>Асуулт 3</h3>
                            <label><input required type="radio" name="q3" value="A">
                                A</label>
                            <label><input required type="radio" name="q3" value="B">
                                B</label>
                            <label><input required type="radio" name="q3" value="C">
                                C</label>
                            <label><input required type="radio" name="q3" value="D">
                                D</label>
                        </div>
                        <div class="options">
                            <h3>Асуулт 4</h3>
                            <label><input required type="radio" name="q4" value="A">
                                A</label>
                            <label><input required type="radio" name="q4" value="B">
                                B</label>
                            <label><input required type="radio" name="q4" value="C">
                                C</label>
                            <label><input required type="radio" name="q4" value="D">
                                D</label>
                        </div>
                        <div class="options">
                            <h3>Асуулт 5</h3>
                            <label><input required type="radio" name="q5" value="A">
                                A</label>
                            <label><input required type="radio" name="q5" value="B">
                                B</label>
                            <label><input required type="radio" name="q5" value="C">
                                C</label>
                            <label><input required type="radio" name="q5" value="D">
                                D</label>
                        </div>
                        <div class="options">
                            <h3>Асуулт 6</h3>
                            <label><input required type="radio" name="q6" value="A">
                                A</label>
                            <label><input required type="radio" name="q6" value="B">
                                B</label>
                            <label><input required type="radio" name="q6" value="C">
                                C</label>
                            <label><input required type="radio" name="q6" value="D">
                                D</label>
                        </div>
                        <div class="options">
                            <h3>Асуулт 7</h3>
                            <label><input required type="radio" name="q7" value="A">
                                A</label>
                            <label><input required type="radio" name="q7" value="B">
                                B</label>
                            <label><input required type="radio" name="q7" value="C">
                                C</label>
                            <label><input required type="radio" name="q7" value="D">
                                D</label>
                        </div>
                        <div class="options">
                            <h3>Асуулт 8</h3>
                            <label><input required type="radio" name="q8" value="A">
                                A</label>
                            <label><input required type="radio" name="q8" value="B">
                                B</label>
                            <label><input required type="radio" name="q8" value="C">
                                C</label>
                            <label><input required type="radio" name="q8" value="D">
                                D</label>
                        </div>
                        <div class="options">
                            <h3>Асуулт 9</h3>
                            <label><input required type="radio" name="q9" value="A">
                                A</label>
                            <label><input required type="radio" name="q9" value="B">
                                B</label>
                            <label><input required type="radio" name="q9" value="C">
                                C</label>
                            <label><input required type="radio" name="q9" value="D">
                                D</label>
                        </div>
                        <div class="options">
                            <h3>Асуулт 10</h3>
                            <label><input required type="radio" name="q10" value="A">
                                A</label>
                            <label><input required type="radio" name="q10" value="B">
                                B</label>
                            <label><input required type="radio" name="q10" value="C">
                                C</label>
                            <label><input required type="radio" name="q10" value="D">
                                D</label>
                        </div>
                        <div class="options">
                            <h3>Асуулт 11</h3>
                            <label><input required type="radio" name="q11" value="A">
                                A</label>
                            <label><input required type="radio" name="q11" value="B">
                                B</label>
                            <label><input required type="radio" name="q11" value="C">
                                C</label>
                            <label><input required type="radio" name="q11" value="D">
                                D</label>
                        </div>
                        <div class="options">
                            <h3>Асуулт 12</h3>
                            <label><input required type="radio" name="q12" value="A">
                                A</label>
                            <label><input required type="radio" name="q12" value="B">
                                B</label>
                            <label><input required type="radio" name="q12" value="C">
                                C</label>
                            <label><input required type="radio" name="q12" value="D">
                                D</label>
                        </div>
                        <div class="options">
                            <h3>Асуулт 13</h3>
                            <label><input required type="radio" name="q13" value="A">
                                A</label>
                            <label><input required type="radio" name="q13" value="B">
                                B</label>
                            <label><input required type="radio" name="q13" value="C">
                                C</label>
                            <label><input required type="radio" name="q13" value="D">
                                D</label>
                        </div>
                        <div class="options">
                            <h3>Асуулт 14</h3>
                            <label><input required type="radio" name="q14" value="A">
                                A</label>
                            <label><input required type="radio" name="q14" value="B">
                                B</label>
                            <label><input required type="radio" name="q14" value="C">
                                C</label>
                            <label><input required type="radio" name="q14" value="D">
                                D</label>
                        </div>
                        <div class="options">
                            <h3>Асуулт 15</h3>
                            <label><input required type="radio" name="q15" value="A">
                                A</label>
                            <label><input required type="radio" name="q15" value="B">
                                B</label>
                            <label><input required type="radio" name="q15" value="C">
                                C</label>
                            <label><input required type="radio" name="q15" value="D">
                                D</label>
                        </div>
                        <div class="options">
                            <h3>Асуулт 16</h3>
                            <label><input required type="radio" name="q16" value="A">
                                A</label>
                            <label><input required type="radio" name="q16" value="B">
                                B</label>
                            <label><input required type="radio" name="q16" value="C">
                                C</label>
                            <label><input required type="radio" name="q16" value="D">
                                D</label>
                        </div>
                        <div class="options">
                            <h3>Асуулт 17</h3>
                            <label><input required type="radio" name="q17" value="A">
                                A</label>
                            <label><input required type="radio" name="q17" value="B">
                                B</label>
                            <label><input required type="radio" name="q17" value="C">
                                C</label>
                            <label><input required type="radio" name="q17" value="D">
                                D</label>
                        </div>
                        <div class="options">
                            <h3>Асуулт 18</h3>
                            <label><input required type="radio" name="q18" value="A">
                                A</label>
                            <label><input required type="radio" name="q18" value="B">
                                B</label>
                            <label><input required type="radio" name="q18" value="C">
                                C</label>
                            <label><input required type="radio" name="q18" value="D">
                                D</label>
                        </div>
                        <div class="options">
                            <h3>Асуулт 19</h3>
                            <label><input required type="radio" name="q19" value="A">
                                A</label>
                            <label><input required type="radio" name="q19" value="B">
                                B</label>
                            <label><input required type="radio" name="q19" value="C">
                                C</label>
                            <label><input required type="radio" name="q19" value="D">
                                D</label>
                        </div>
                        <div class="options">
                            <h3>Асуулт 20</h3>
                            <label><input required type="radio" name="q20" value="A">
                                A</label>
                            <label><input required type="radio" name="q20" value="B">
                                B</label>
                            <label><input required type="radio" name="q20" value="C">
                                C</label>
                            <label><input required type="radio" name="q20" value="D">
                                D</label>
                        </div>


                    </div>

                    <div class="footer">
                        <div class="last-input">
                            <p> Нэр оруулах:
)";
String customer1 = R"(
                            </p>
                            <input type="text" name="name">
                        </div>
                        <div class="btn">
                            <button type="submit" class="submit-btn">Илгээх</button>
                        </div>
                    </div>

                </form>

            </div>
        </div>
    </div>

    </div>

</body>
)";

const char* admin = R"(
<!DOCTYPE html>
<html>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<link rel="stylesheet" href="/admin.css">

<head>

    <title>Exam Page</title>
</head>
<style>
    * {
        box-sizing: border-box;
        margin: 0;
        padding: 0;
    }

    body {
        font-family: Arial, sans-serif;
        background-color: rgb(77, 252, 182);
    }



    .bg-image-1 {
        /* background-image: url("./bg-image-2.jpg");
    background-repeat: no-repeat;
    background-size: cover;
    background-position: center; */
        width: 100vw;
        display: flex;
        justify-content: center;
    }

    .bg-image-1 .opacity {
        height: 100vh;
        width: 100vw;
        background-color: #fff;
        opacity: 0.3;
        z-index: 1;
    }

    .bg-image-1 .container {
        top: 500px;
        position: relative;
        z-index: 2;
        background-color: #fff;
        border-radius: 6px;
        width: 300px;
        height: 800px;
        display: flex;
        justify-content: center;
        border: 1px darkcyan dashed;
    }

    .bg-image-1 .container form {
        height: 800px;
        width: 240px;
        overflow-y: scroll;
        overflow-anchor: none;
        text-align: center;
        background-color: aqua;
    }


    .bg-image-1 .container form button {
        height: 30px;
        width: 360px;
        padding: 5px 80px;
        margin: 20px 0;
        border: 1px darkcyan dashed;
        background-color: rgb(77, 252, 182);
        transition: all 0.5s;

    }

    .bg-image-1 .container form button:hover {
        background-color: rgb(16, 111, 73);
        transition: all 0.5s;
        color: #fff;
        border-radius: 6px;
    }

    .bg-image {
        /* background-image: url("./bg-image.jpg");
    background-repeat: no-repeat;
    background-size: cover;
    background-position: center; */
        width: 100vw;
        display: flex;
        justify-content: center;

    }



    .bg-image .form-container-1 {
        margin-top: 20px;
        position: absolute;
        z-index: 2;
        background-color: transparent;
        width: 700px;
        height: 460px;
        background-color: #fff;
        border-radius: 6px;
        display: flex;
        flex-direction: column;
        justify-content: center;
        align-items: center;
        border: 1px darkcyan dashed;
    }

    .bg-image .form-container-1 form:first-child h3 {
        text-align: center;
    }

    .bg-image .form-container-1 form:first-child input {
        border: 1px darkcyan dashed;
        height: 30px;
        border-radius: 6px;
    }

    .bg-image .form-container-1 form {
        display: flex;
        flex-direction: column;
        width: 200px;
    }

    .bg-image .form-container-1 form button {
        margin-top: 30px;
        border: 1px darkcyan dashed;
        height: 30px;
        border-radius: 6px;
        background-color: rgb(222, 222, 222);
        transition: all 0.6s;
    }

    .bg-image .form-container-1 form button:hover {
        background-color: rgb(138, 138, 138);
        color: #ffF;
        transition: all 0.6s;
    }

    @media screen and (max-width:568px) {
        .bg-image .form-container-1 {
            margin-top: 20px;
            position: absolute;
            z-index: 2;
            width: 300px;
            height: 400px;
            background-color: #fff;
            border-radius: 6px;
            display: flex;
            flex-direction: column;
            justify-content: center;
            align-items: center;
            border: 1px darkcyan dashed;
        }

        .bg-image .form-container-1 form:first-child h3 {
            text-align: center;
            margin: 10px 0;
        }

        /* 
    .bg-image .form-container-1 form:first-child {

    } */

        .bg-image .form-container-1 form:first-child input {
            border: 1px darkcyan dashed;
            height: 25px;
            border-radius: 6px;
        }

        .bg-image .form-container-1 form {
            display: flex;
            flex-direction: column;
            width: 200px;
        }

        .bg-image .form-container-1 form button {
            margin-top: 14px;
            border: 1px darkcyan dashed;
            height: 30px;
            border-radius: 6px;
            background-color: rgb(77, 252, 182);
            transition: all 0.5s;
        }

        .bg-image .form-container-1 form button:hover {
            background-color: rgb(16, 111, 73);
            transition: all 0.5s;
            color: #fff;
        }

        .bg-image-1 .container {
            top: 430px;
            position: absolute;
            background-color: #fff;
            border-radius: 6px;
            width: 370px;
            height: 350px;
            display: flex;
            justify-content: center;
            border: 1px darkcyan dashed;
        }

        .bg-image-1 .container form {
            height: 350px;
            width: 400px;
            /* overflow-y: scroll;
        overflow-anchor: none; */
            text-align: center;
            justify-content: center;
            display: grid;
            grid-template-columns: repeat(1, 1fr);
            position: relative;
        }

        .bg-image-1 .container form .options {
            margin-right: 7px;
        }

        .bg-image-1 .container form .options label {
            padding: 2px 8px;
        }

        .bg-image-1 .container form .options h3 {
            margin-top: 6px;
            margin-bottom: 7px;
        }

        .bg-image-1 .container form button {
            padding: 8px 80px;
            margin: 20px 0;
            border: 1px darkcyan dashed;
            background-color: rgb(77, 252, 182);
            transition: all 0.5s;

        }

        .bg-image-1 .container form button:hover {
            background-color: rgb(16, 111, 73);
            transition: all 0.5s;
            color: #fff;
            border-radius: 6px;
        }

    }
</style>

<body>
    <div class="bg-image">
        <div class="form-container-1">
            <form action="/upload" method="POST">
                <h3> WiFi SSID</h3>
                <input type="text" name="ssid" value="wifi нэр оруулна уу." required>
                <h3> WiFi PASS</h3>
                <input type="text" name="pass" value="Пасспортоо оруулна уу." required>
                <button type="submit" class="submit-btn"> Холбогдох </button>
            </form>
            <!--
            <form action="/upload" method="GET">
                <button type="submit" class="submit-btn"> Файл оруулах </button>
            </form>
            -->
            <form action="/push_data" method="GET">
                <button type="submit" class="submit-btn"> Шалгалт дуусгах </button>
            </form>
            <form action="/data_harah" method="GET">
                <button type="submit" class="submit-btn"> Шалгалтын дүн </button>
            </form>

        </div>

    </div>
    <div class="bg-image-1">
        <div class="container">
            <form action="/admin" method="GET">
                <div class="options">
                    <h3>Асуулт 1</h3>
                    <label><input type="radio" name="a1" value="A" required> A</label>
                    <label><input type="radio" name="a1" value="B" required> B</label>
                    <label><input type="radio" name="a1" value="C" required> C</label>
                    <label><input type="radio" name="a1" value="D" required> D</label>
                </div>
                <div class="options">
                    <h3>Асуулт 2</h3>
                    <label><input type="radio" name="a2" value="A" required> A</label>
                    <label><input type="radio" name="a2" value="B" required> B</label>
                    <label><input type="radio" name="a2" value="C" required> C</label>
                    <label><input type="radio" name="a2" value="D" required> D</label>
                </div>
                <div class="options">
                    <h3>Асуулт 3</h3>
                    <label><input type="radio" name="a3" value="A" required> A</label>
                    <label><input type="radio" name="a3" value="B"> B</label>
                    <label><input type="radio" name="a3" value="C"> C</label>
                    <label><input type="radio" name="a3" value="D"> D</label>
                </div>
                <div class="options">
                    <h3>Асуулт 4</h3>
                    <label><input type="radio" name="a4" value="A"> A</label>
                    <label><input type="radio" name="a4" value="B"> B</label>
                    <label><input type="radio" name="a4" value="C"> C</label>
                    <label><input type="radio" name="a4" value="D"> D</label>
                </div>
                <div class="options">
                    <h3>Асуулт 5</h3>
                    <label><input type="radio" name="a5" value="A"> A</label>
                    <label><input type="radio" name="a5" value="B"> B</label>
                    <label><input type="radio" name="a5" value="C"> C</label>
                    <label><input type="radio" name="a5" value="D"> D</label>
                </div>
                <div class="options">
                    <h3>Асуулт 6</h3>
                    <label><input type="radio" name="a6" value="A"> A</label>
                    <label><input type="radio" name="a6" value="B"> B</label>
                    <label><input type="radio" name="a6" value="C"> C</label>
                    <label><input type="radio" name="a6" value="D"> D</label>
                </div>
                <div class="options">
                    <h3>Асуулт 7</h3>
                    <label><input type="radio" name="a7" value="A"> A</label>
                    <label><input type="radio" name="a7" value="B"> B</label>
                    <label><input type="radio" name="a7" value="C"> C</label>
                    <label><input type="radio" name="a7" value="D"> D</label>
                </div>
                <div class="options">
                    <h3>Асуулт 8</h3>
                    <label><input type="radio" name="a8" value="A"> A</label>
                    <label><input type="radio" name="a8" value="B"> B</label>
                    <label><input type="radio" name="a8" value="C"> C</label>
                    <label><input type="radio" name="a8" value="D"> D</label>
                </div>
                <div class="options">
                    <h3>Асуулт 9</h3>
                    <label><input type="radio" name="a9" value="A"> A</label>
                    <label><input type="radio" name="a9" value="B"> B</label>
                    <label><input type="radio" name="a9" value="C"> C</label>
                    <label><input type="radio" name="a9" value="D"> D</label>
                </div>
                <div class="options">
                    <h3>Асуулт 10</h3>
                    <label><input type="radio" name="a10" value="A"> A</label>
                    <label><input type="radio" name="a10" value="B"> B</label>
                    <label><input type="radio" name="a10" value="C"> C</label>
                    <label><input type="radio" name="a10" value="D"> D</label>
                </div>
                <div class="options">
                    <h3>Асуулт 11</h3>
                    <label><input type="radio" name="a11" value="A"> A</label>
                    <label><input type="radio" name="a11" value="B"> B</label>
                    <label><input type="radio" name="a11" value="C"> C</label>
                    <label><input type="radio" name="a11" value="D"> D</label>
                </div>
                <div class="options">
                    <h3>Асуулт 12</h3>
                    <label><input type="radio" name="a12" value="A"> A</label>
                    <label><input type="radio" name="a12" value="B"> B</label>
                    <label><input type="radio" name="a12" value="C"> C</label>
                    <label><input type="radio" name="a12" value="D"> D</label>
                </div>
                <div class="options">
                    <h3>Асуулт 13</h3>
                    <label><input type="radio" name="a13" value="A"> A</label>
                    <label><input type="radio" name="a13" value="B"> B</label>
                    <label><input type="radio" name="a13" value="C"> C</label>
                    <label><input type="radio" name="a13" value="D"> D</label>
                </div>
                <div class="options">
                    <h3>Асуулт 14</h3>
                    <label><input type="radio" name="a14" value="A"> A</label>
                    <label><input type="radio" name="a14" value="B"> B</label>
                    <label><input type="radio" name="a14" value="C"> C</label>
                    <label><input type="radio" name="a14" value="D"> D</label>
                </div>
                <div class="options">
                    <h3>Асуулт 15</h3>
                    <label><input type="radio" name="a15" value="A"> A</label>
                    <label><input type="radio" name="a15" value="B"> B</label>
                    <label><input type="radio" name="a15" value="C"> C</label>
                    <label><input type="radio" name="a15" value="D"> D</label>
                </div>
                <div class="options">
                    <h3>Асуулт 16</h3>
                    <label><input type="radio" name="a16" value="A"> A</label>
                    <label><input type="radio" name="a16" value="B"> B</label>
                    <label><input type="radio" name="a16" value="C"> C</label>
                    <label><input type="radio" name="a16" value="D"> D</label>
                </div>
                <div class="options">
                    <h3>Асуулт 17</h3>
                    <label><input type="radio" name="a17" value="A"> A</label>
                    <label><input type="radio" name="a17" value="B"> B</label>
                    <label><input type="radio" name="a17" value="C"> C</label>
                    <label><input type="radio" name="a17" value="D"> D</label>
                </div>
                <div class="options">
                    <h3>Асуулт 18</h3>
                    <label><input type="radio" name="a18" value="A"> A</label>
                    <label><input type="radio" name="a18" value="B"> B</label>
                    <label><input type="radio" name="a18" value="C"> C</label>
                    <label><input type="radio" name="a18" value="D"> D</label>
                </div>
                <div class="options">
                    <h3>Асуулт 19</h3>
                    <label><input type="radio" name="a19" value="A"> A</label>
                    <label><input type="radio" name="a19" value="B"> B</label>
                    <label><input type="radio" name="a19" value="C"> C</label>
                    <label><input type="radio" name="a19" value="D"> D</label>
                </div>
                <div class="options">
                    <h3>Асуулт 20</h3>
                    <label><input type="radio" name="a20" value="A"> A</label>
                    <label><input type="radio" name="a20" value="B"> B</label>
                    <label><input type="radio" name="a20" value="C"> C</label>
                    <label><input type="radio" name="a20" value="D"> D</label>
                </div>
                <div class="btn">
                    <button type="submit" class="submit-btn">Илгээх</button>
                </div>
            </form>
        </div>

    </div>



</body>

</html>
)";

File file;
String customertxt;

void handleUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final){
  Serial.println("YES");
  if(!index){
    //filename.c_str()
    Serial.printf("UploadStart: %s\n", "/example.pdf");
  }
  
  for(size_t i=0; i<len; i++){
    Serial.write(data[i]);
  }

  if(final){
    Serial.printf("UploadEnd: %s, %u B\n", "/example.pdf", index+len);
  }
}

String grade(String admin, String custom, bool bl) {
  int cur = 0;
  String answer;
  for (int i = 0; i < admin.length(); i++) {
    if (admin[i] == custom[i]) cur++;
  }
  Serial.println(bl);
  if (bl == 1) {
    Serial.println(bl);
    answer = String(admin.length()) + "/" + String(cur);

    return answer;
  }
  else {
    return "0";
  }

  Serial.println();
}

void setup() {
  Serial.begin(115200);

  if (!LittleFS.begin()) {
    Serial.println("Failed to mount LittleFS");
    return;
  }

  WiFi.softAP(ssid, password);
  Serial.println("Access Point Started");
  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    //readFile(LittleFS, "/");
    listDir(LittleFS ,"/", 1);
    Serial.println("/");
    request->send(200, "text/html", login);
    Serial.println(readFile(LittleFS, "/admin.txt"));
    Serial.println(readFile(LittleFS, "/customer.txt"));
  });

  // Handle login
  server.on("/login", HTTP_GET, [](AsyncWebServerRequest* request) {
    listDir(LittleFS ,"/", 1);
    String username = request->arg("username");
    String password = request->arg("password");

    if (username == adminUsername && password == adminPassword) {
      //request->send(LittleFS, "/admin.html", "text/html");
      request->send(200, "text/html", admin);
    } else if (username == customerUsername && password == customerPassword) {
      //request->send(LittleFS, "/customer.html", "text/html");
      request->send(200, "text/html", customer + customer1);
    } else {
      request->send(401, "text/plain", "Invalid credentials!");
    }
  });

  // Admin page to set exam
  server.on("/admin", HTTP_GET, [](AsyncWebServerRequest* request) {
    Serial.println("/admin");

    String a;
    String st;
    String admintxt = "";
    for (int i = 1; i <= 20; i++) {
      a = "a";
      a += String(i);

      Serial.println(a);
      st = request->arg(a);

      admintxt += st; 
    }

    writeFile(LittleFS, "/admin.txt", admintxt);

    request->send(200, "text/html", admin);
  });

  // Customer page to answer exam
  server.on("/customer", HTTP_GET, [](AsyncWebServerRequest* request) {
    String q;
    String st;
    String lastans;
    String admin = readFile(LittleFS, "/admin.txt");
    customertxt += request->arg("name") + "-";
    for (int i = 1; i <= 20; i++) {
      q = "q";
      q += String(i);

      Serial.println(q);
      st = request->arg(q);

      customertxt += st;
      lastans += request->arg(q);
    }

    customertxt += ",";

    request->send(200, "text/html", customer + grade(admin, lastans, 1) + customer1);
  });

  server.on("/push_data", HTTP_GET, [](AsyncWebServerRequest* request) {
    writeFile(LittleFS, "/customer.txt", customertxt);
    customertxt = "";
    request->send(200, "text/html", admin);
  });

  /*
  <div style="display: flex; justify-content: center;align-items: center;height: 100vh;">
	<table border="1px" style="border: 1px solid #20cfa0; border-collapse: collapse ;">
		<tr><td style="padding: 8px;">aa</td><td style="padding: 8px;">aa</td><td style="padding: 8px;">aa</td><td style="padding: 8px;">aa</td></tr>
	</table>
  </div>
*/

  server.on("/data_harah", HTTP_GET, [](AsyncWebServerRequest* request) {
    String datas = readFile(LittleFS, "/customer.txt");
    String admin = readFile(LittleFS, "/admin.txt");
    String web;
    String name;
    String qs;
    bool bl = false;

    web += "<meta charset='UTF-8'>";
    web += "<div style='display: flex; justify-content: center;align-items: center;height: 100vh;'>";
    web += "<table border='1px' style='border: 1px solid #20cfa0; border-collapse: collapse ;'>";
    web += "<tr><td style='padding: 8px;'> Нэр </td><td style='padding: 8px;'>Таний хариулт</td><td style='padding: 8px;'>Шалгалтын хариу</td><td style='padding: 8px;'>Шалгалтын хувь</td></tr>";

    for (int i = 0; i < datas.length(); i++) {
      if (datas[i] == ',') {
        //Serial.println(name, qs);
        //Serial.print(name);
        //Serial.println(qs);
        web += "<tr><td style='padding: 8px;'>" + name + "</td><td style='padding: 8px;'>" + qs + "</td><td style='padding: 8px;'>" + grade(admin, qs, 1) + "</td><td style='padding: 8px;'>" + grade(admin, qs, 0) + "</td></tr>";
        name.clear();
        qs.clear();
        bl = false;

        continue;
      }

      if (!bl && datas[i] != '-') name += datas[i];
      else if (bl) qs += datas[i];
      
      if (datas[i] == '-') bl = true;

    }

    web += "</table></div>";

    request->send(200, "text/html", web);
  });

  server.on("/example.pdf", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(LittleFS, "/example.pdf", "application/pdf");
  });
  /*
  server.on("/upload", HTTP_POST, [](AsyncWebServerRequest* request) {
    const String &Ssid = request->arg("ssid");
    const String &Pass = request->arg("pass");
    file = LittleFS.open("/example.pdf", FILE_WRITE);
    //Serial.println(Ssid, Pass);

    //listDir(LittleFS ,"/", 1);
    request->send(200, "text/html", serverIndex);
  }, handleUpload);
  */

  server.on("/upload", HTTP_POST, [](AsyncWebServerRequest *request) {
    if(request->hasParam("data", true, true) && LittleFS.exists(request->getParam("data", true, true)->value()))
      request->send(200, "", "UPLOADED: "+request->getParam("data", true, true)->value());
  });
  

  server.on("/doUpload", HTTP_POST, [](AsyncWebServerRequest *request) {},
      [](AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data,
                    size_t len, bool final) {handleUpload(request, filename, index, data, len, final);}
  );

  /*
  server.on("/jquery.min.js", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(LittleFS, "/jquery.min.js", "text/javascript");
  });
  */

    //writeFile(LittleFS, "/result.txt", "Hello ");
    //readFile(LittleFS, "/result.txt");

  server.begin();
}

void writeFile(fs::FS &fs, const char * path, String message){
  Serial.printf("Writing file: %s\r\n", path);

  File file = fs.open(path, FILE_WRITE);
  if(!file){
    Serial.println("- failed to open file for writing");
    return;
  }
  if(file.print(message)){
    Serial.println("- file written");
  } else {
    Serial.println("- write failed");
  }
  file.close();
}


void wf(fs::FS &fs, const char * path, uint8_t *data, size_t size){
  Serial.printf("Writing file: %s\r\n", path);

  File file = fs.open(path, FILE_WRITE);
  if(!file){
    Serial.println("- failed to open file for writing");
    return;
  }
  if(file.write(data, size)){
    Serial.println("- file written");
  } else {
    Serial.println("- write failed");
  }
  file.close();
}


String readFile(fs::FS &fs, const char * path){
  Serial.printf("Reading file: %s\r\n", path);

  File file = fs.open(path);
  if(!file || file.isDirectory()){
    Serial.println("- failed to open file for reading");
    return "";
  }

  String st;
  Serial.println("- read from file:");
  while(file.available()){
    st += file.readString();
    //Serial.println(file.readString());
    //Serial.write(file.read());
  }
  file.close();

  return st;
}

void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
    Serial.printf("Listing directory: %s\r\n", dirname);

    File root = fs.open(dirname);
    if(!root){
        Serial.println("- failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        Serial.println(" - not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if(levels){
                listDir(fs, file.path(), levels -1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("\tSIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}

void loop() {
  // Nothing to do here
}
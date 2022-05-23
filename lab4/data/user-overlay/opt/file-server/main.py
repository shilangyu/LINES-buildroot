import threading
from http.client import OK
from pathlib import Path

from flask import Flask, render_template

import gpio
from util import login_required

app = Flask(__name__)


@app.route("/")
def index():
    with open(gpio.logs_file) as file:
        lines = file.readlines()
        lines = [line.rstrip() for line in lines]
        return render_template('index.html', logs=lines)


@app.route("/clear_logs", methods=['POST'])
@login_required
def upload():
    with open(gpio.logs_file, 'w') as f:
        f.close()

    return "ok", OK


if __name__ == "__main__":
    gpio.init_gpio()
    th = threading.Thread(target=gpio.listen_motion)
    th.start()
    app.run(host='0.0.0.0')
    th.join()

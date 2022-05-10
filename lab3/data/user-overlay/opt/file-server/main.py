import os
import sys
from http.client import BAD_REQUEST, OK
from pathlib import Path

from flask import Flask, render_template, request, send_file

from util import login_required

app = Flask(__name__)
root: Path


@app.route("/")
def index():
    files = [f for f in root.glob("*") if f.is_file()]

    return render_template('index.html', files=files)


@app.route("/download/<path:file>")
def download(file: str):
    return send_file(f'/{file}')


@app.route("/upload", methods=['POST'])
@login_required
def upload():
    file = request.files['file']

    if file.filename == '':
        return "bad request", BAD_REQUEST
    else:
        file.save(os.path.join(root, file.filename))

    return "ok", OK


if __name__ == "__main__":
    if len(sys.argv) != 2:
        raise Exception("Provide folder path")
    root = Path(sys.argv[1])
    root.mkdir(parents=True, exist_ok=True)

    app.run(host='0.0.0.0')

from functools import wraps

from flask import request


# source: https://stackoverflow.com/a/50102372/7907549
def login_required(f):
    @wraps(f)
    def wrapped_view(**kwargs):
        auth = request.authorization
        if not (auth and auth.username == "admin" and auth.password == "admin"):
            return ('Unauthorized', 401, {
                'WWW-Authenticate': 'Basic realm="Login Required"'
            })

        return f(**kwargs)

    return wrapped_view

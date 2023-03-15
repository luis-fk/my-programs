import flask

# import blue prints for the different pages
from blueprints.collatz.collatz import collatz
from blueprints.crossword.crossword import crossword
from blueprints.numlee.numlee import numlee

app = flask.Flask(__name__)
app.secret_key = "secret-key"

# register the different blueprints
app.register_blueprint(collatz, url_prefix="/")
app.register_blueprint(crossword, url_prefix="/")
app.register_blueprint(numlee, url_prefix="/")

# main page of the site
@app.route("/", methods=["GET", "POST"])
def index():
    return flask.render_template("index.html")


if __name__ == "__main__":
    app.run(debug=True)

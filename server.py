# 3rd party moudles
from flask import render_template

# Local modules
import config


# Get the application instance
connex_app = config.connex_app

# Read the swagger.yml file to configure the endpoints
connex_app.add_api("steamgames_api.v1.yml")


# Create a URL route in our application for "/"
@connex_app.route("/")
def home():
    """
    This function just responds to the browser URL
    localhost:5000/

    :return:        the rendered template "home.html"
    """
    return render_template("home.html")
@connex_app.route("/users")
def people():
    """
    This function just responds to the browser URL
    localhost:5000/people

    :return:        the rendered template "people.html"
    """
    return render_template("home.html")
# Create a URL route in our application for "/people"
@connex_app.route("/reviews")
def reviews():
    """
    This function just responds to the browser URL
    localhost:5000/people

    :return:        the rendered template "people.html"
    """
    return render_template("reviews.html")
@connex_app.route("/games")
def games():
    """
    This function just responds to the browser URL
    localhost:5000/people

    :return:        the rendered template "people.html"
    """
    return render_template("games.html")
@connex_app.route("/genres")
def people():
    """
    This function just responds to the browser URL
    localhost:5000/people

    :return:        the rendered template "people.html"
    """
    return render_template("genres.html")

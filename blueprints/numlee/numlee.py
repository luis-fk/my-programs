import flask
import subprocess
import os
import random

# declaring the blueprint
numlee = flask.Blueprint("numlee", __name__,
                        static_folder="static",
                        template_folder="templates", 
                        static_url_path="blueprints/numlee/static")

# declaring the number variable that will be used in multiple functions
number = 0

@numlee.route("/numlee1", methods=["GET", "POST"])
def numlee1():
    if flask.request.method == "GET":
        os.chdir("blueprints/numlee")
        # clear the file so it can be written onto again
        open("numlee.txt", "w").close()
        os.chdir("../..")

        return flask.render_template("/numlee1.html")
    else:
        global number
        # generates random number and get the guess from the user
        number = random.randint(1, 99999)
        guess = flask.request.form.get("guess")

        # if the guess is out or range, is negative or is not a number, an
        # error message will be printed
        if not guess.isdigit() or int(guess) > 99999 or int(guess) == 0:
            flask.flash("Input must be a positive number between 1 and 99999.", "error")
            return flask.redirect("/numlee1")

        # run the C program and reads from the text file
        os.chdir("blueprints/numlee")
        subprocess.call(f"./numlee.exe {number} {guess}")

        with open("numlee.txt", "r") as file:
            text = file.readlines()
        os.chdir("../..")

        check = str(text)

        # check if guess is correct
        if "got" in check:
            flask.flash(f"You got it! The number was {number}.", "info")
            return flask.render_template("/numlee_final.html", text=text)

        return flask.render_template("/numlee2.html", text=text)


@numlee.route("/numlee2", methods=["GET", "POST"])
def numlee2():
    if flask.request.method == "GET":
        return flask.render_template("/numlee2.html")
    else:
        guess = flask.request.form.get("guess")

        # if the guess is out or range, is negative or is not a number, an
        # error message will be printed
        if not guess.isdigit() or int(guess) > 99999:
            flask.flash("Input must be a positive number between 1 and 99999.", "error")
            return flask.redirect("/numlee2")

        # run the C program and reads from the text file
        os.chdir("blueprints/numlee")
        subprocess.call(f"./numlee.exe {number} {guess}")

        with open("numlee.txt", "r") as file:
            text = file.readlines()
        os.chdir("../..")

        check = str(text)

        # check if guess is correct
        if "got" in check:
            flask.flash(f"You got it! The number was {number}.", "info")
            return flask.render_template("/numlee_final.html", text=text)

        return flask.render_template("/numlee3.html", text=text)


@numlee.route("/numlee3", methods=["GET", "POST"])
def numlee3():
    if flask.request.method == "GET":
        return flask.render_template("/numlee3.html")
    else:
        guess = flask.request.form.get("guess")

        # if the guess is out or range, is negative or is not a number, an
        # error message will be printed
        if not guess.isdigit() or int(guess) > 99999:
            flask.flash("Input must be a positive number between 1 and 99999.", "error")
            return flask.redirect("/numlee3")

        # run the C program and reads from the text file
        os.chdir("blueprints/numlee")
        subprocess.call(f"./numlee.exe {number} {guess}")

        with open("numlee.txt", "r") as file:
            text = file.readlines()
        os.chdir("../..")

        check = str(text)

        # check if guess is correct
        if "got" in check:
            flask.flash(f"You got it! The number was {number}.", "info")
            return flask.render_template("/numlee_final.html", text=text)

        return flask.render_template("/numlee4.html", text=text)


@numlee.route("/numlee4", methods=["GET", "POST"])
def numlee4():
    if flask.request.method == "GET":
        return flask.render_template("/numlee4.html")
    else:
        guess = flask.request.form.get("guess")

        # if the guess is out or range, is negative or is not a number, an
        # error message will be printed
        if not guess.isdigit() or int(guess) > 99999:
            flask.flash("Input must be a positive number between 1 and 99999.", "error")
            return flask.redirect("/numlee4")

        # run the C program and reads from the text file
        os.chdir("blueprints/numlee")
        subprocess.call(f"./numlee.exe {number} {guess}")

        with open("numlee.txt", "r") as file:
            text = file.readlines()
        os.chdir("../..")

        check = str(text)

        # check if guess is correct
        if "got" in check:
            flask.flash(f"You got it! The number was {number}.", "info")
            return flask.render_template("/numlee_final.html", text=text)

        return flask.render_template("/numlee5.html", text=text)


@numlee.route("/numlee5", methods=["GET", "POST"])
def numlee5():
    if flask.request.method == "GET":
        return flask.render_template("/numlee5.html")
    else:
        guess = flask.request.form.get("guess")

        # if the guess is out or range, is negative or is not a number, an
        # error message will be printed
        if not guess.isdigit() or int(guess) > 99999:
            flask.flash("Input must be a positive number between 1 and 99999.", "error")
            return flask.redirect("/numlee5")

        # run the C program and reads from the text file
        os.chdir("blueprints/numlee")
        subprocess.call(f"./numlee.exe {number} {guess}")

        with open("numlee.txt", "r") as file:
            text = file.readlines()
        os.chdir("../..")

        check = str(text)

        # check if guess is correct
        if "got" in check:
            flask.flash(f"You got it! The number was {number}.", "info")
            return flask.render_template("/numlee_final.html", text=text)

        flask.flash(f"Not this time, the number was {number}.", "error")
        return flask.render_template("/numlee_final.html", text=text)

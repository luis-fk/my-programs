import flask
import subprocess
import os

# creates blue print for collatz
collatz = flask.Blueprint("collatz", __name__,
                        static_folder="static",
                        template_folder="templates", 
                        static_url_path="blueprints/collatz/static")

@collatz.route("/collatz", methods=["GET", "POST"])
def collatz_function():
    if flask.request.method == "GET":
        return flask.render_template("collatz.html")
    else:
        # gets number from user
        number = flask.request.form.get("number")
        
        # if the number is 0, not positive or not a number, an error message will be returned
        if not number.isdigit() or int(number) == 0:
            flask.flash("Input must be a positive number.", "error")
            return flask.redirect("/collatz")
        
        # an upper limit for what an unsigned long long int can handle in C
        # a higher value could be used, but 10^16 is a safe bet and big enough
        # for all intents and purposes
        if int(number) > 10**16:
            flask.flash("Number is too large.", "error")
            return flask.redirect("/collatz")

        # directory change to the blueprint folder to acess the text file and
        # the C executable file
        os.chdir("blueprints/collatz")
        subprocess.call(f"./collatz.exe {number}")
        
        with open("collatz.txt", "r") as file:
            steps = file.readlines()
        os.chdir("../..")

        # message that showcases the number of steps for a given number
        num = len(steps) - 1
        flask.flash(f"The number of setps for the number {number} is {steps[num]}.", "info")
        
        if flask.request.form.get("steps"):
            return flask.render_template("collatz.html", steps=steps)
        else:
            return flask.redirect("/collatz")
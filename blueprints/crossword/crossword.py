import flask
import subprocess
import os

# creates blueprint
crossword = flask.Blueprint("crossword", __name__,
                        static_folder="static",
                        template_folder="templates", 
                        static_url_path="blueprints/crossword/static")

# set global variables that will be used in multiple functions
row_number = 0
column_number = 0

@crossword.route("/board_size", methods=["GET", "POST"])
def board_size():
    if flask.request.method == "GET":
        return flask.render_template("/board_size.html")
    else:
        global row_number
        global column_number
        # get row and column number from user
        row_number = flask.request.form.get("rows")
        column_number = flask.request.form.get("columns")

        # write the values above onto a file in another directory
        os.chdir("blueprints/crossword")
        with open("crossword.txt", "w") as file:
            file.write(row_number + " ")
            file.write(column_number + "\n")
        os.chdir("../..")

        # inputs for the board size must be positive and also cannot be characters
        if not row_number.isdigit() or not column_number.isdigit():
            flask.flash("Input must be a positive number.", "error")
            return flask.redirect("/board_size")

        # convert values to ints
        row_number = int(row_number)
        column_number = int(column_number)

        # make a list of rows and columns so they can be iterable with jinja
        rows = [i for i in range(row_number)]
        columns = [i for i in range(column_number)]

        return flask.render_template("/board_layout.html", rows=rows, columns=columns)


@crossword.route("/board_layout", methods=["GET", "POST"])
def board_layout():
    if flask.request.method == "GET":
        return flask.render_template("/board_layout.html")
    else:
        rows = []
        global row_number
        # insert the values of each row into the list rows so that it can
        # be written onto the text file
        for i in range(row_number):
            rows.insert(i, flask.request.form.getlist(f"value{i}"))

        # write the list as is onto a temp text file
        os.chdir("blueprints/crossword")
        with open("crossword_temp.txt", 'w') as file:
            for i in rows:
                file.write(str(i) + "\n")

        # here is where we will apend the values onto the main crossword text file.
        # This was needed because of the way python writes the list of rows into
        # text files, so we need to filter out commas and brackets.
        with open("crossword_temp.txt", "r") as file_1:
            while True:
                char = file_1.read(1)
                if not char:
                    break
                elif char == "]":
                    with open("crossword.txt", "a") as file_2:
                        file_2.write("\n")
                elif char.isdigit():
                    with open("crossword.txt", "a") as file_2:
                        if char == "1":
                            file_2.write("-1 ")
                        else:
                            file_2.write("0 ")
        os.chdir("../..")

        return flask.render_template("/word_list.html")


@crossword.route("/word_list", methods=["GET", "POST"])
def word_list():
    if flask.request.method == "GET":
        return flask.render_template("/word_list.html")
    else:
        words = flask.request.form.get("words")

        # check if all words inputed are ASCII
        if not words.isascii():
            flask.flash("Words must not contain special characters.", "error")
            return flask.redirect("/word_list")

        # count the number of words, that is needed for the C program to run properly
        num_of_words = len(words.split())

        # open the text file from the crossword directory and append the number
        # of words and the list of words given by the user
        os.chdir("blueprints/crossword")
        with open("crossword.txt", "a") as file:
            file.write(str(num_of_words) + "\n")
            file.write(words)

        # run the C program
        subprocess.call("./crossword")
        os.chdir("../..")

        board = {}
        row = 0
        column = 0
        # after the C program is ran, it will ouput a file with the solution fi there is one,
        # in which case we need to create a dictionary called "board" that will enable us to
        # create custom indexes that simulate a 2D board that can be iterable in jinja.
        # In this case then, the indexes will follow the format of 00, 01, 02, 10, 11, 12... 
        os.chdir("blueprints/crossword")
        with open("crossword.txt", "r") as file:
            while True:
                # reads character by character
                temp = file.read(1)
                if not temp:
                    break
                elif temp == "\n":
                    # set the indexes
                    row += 1
                    column = 0
                    continue
                else:
                    # add character to the index of the dictionary
                    board[f"{row}{column}"] = temp
                    column += 1

        solution = []
        # this will open the file again and read it into solution,
        # which will then check below if there is a solution or not
        with open("crossword.txt", "r") as file:
            solution = file.readlines()
        os.chdir("../..")
        
        # check if there is a solution
        if "There is no solution." in solution:
            flask.flash("There is no solution.", "error")
            return flask.render_template("/no_solution.html")

        # below a rows and columns lists are created so they can be iterable in jinja
        rows = []
        columns = []
        for i in range(row_number):
            rows.append(i)

        for i in range(column_number):
            columns.append(i)
        
        return flask.render_template("/solution.html", board=board, rows=rows, columns=columns)
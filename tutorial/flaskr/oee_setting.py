from flask import Blueprint
from flask import flash
from flask import g
from flask import redirect
from flask import render_template
from flask import request
from flask import url_for
from werkzeug.exceptions import abort

from flaskr.auth import login_required
from flaskr.db import get_db
import flaskr.mysqldb as mydb

from flask import current_app

import traceback
import json

bp = Blueprint("oee_setting", __name__)

@bp.route("/oee_setting")
def index():

    """Show all the posts, most recent first."""
    db = get_db()
    posts = db.execute(
        "SELECT p.id, title, body, created, author_id, username"
        " FROM post p JOIN user u ON p.author_id = u.id"
        " ORDER BY created DESC"
    ).fetchall()
    colours = ['Red', 'Blue', 'Black', 'Orange']
    try:
        mydb.print_connector()
        mydb.insert_machine("M02","23456")
        machines = mydb.get_machines()
        print(machines)
        for e in machines:
            print(json.dumps(machines, sort_keys=True, indent=4, separators=(',', ': '), default=str))
        print(posts)
    except:
        traceback.print_exc()
    
    return render_template("oee/oee_setting.html", machines=machines,posts=posts,colours=colours)
 #
 
def get_machine(id, check_author=True):
    """Get a post and its author by id.

    Checks that the id exists and optionally that the current user is
    the author.

    :param id: id of post to get
    :param check_author: require the current user to be the author
    :return: the post with author information
    :raise 404: if a post with the given id doesn't exist
    :raise 403: if the current user isn't the author
    """
    post = (
        get_db()
        .execute(
            "SELECT p.id, title, body, created, author_id, username"
            " FROM post p JOIN user u ON p.author_id = u.id"
            " WHERE p.id = ?",
            (id,),
        )
        .fetchone()
    )

    if post is None:
        abort(404, "Post id {0} doesn't exist.".format(id))

    if check_author and post["author_id"] != g.user["id"]:
        abort(403)

    return post
from flask import Flask
app = Flask(__name__)


@app.route('/user/create', methods=['POST'])
def create_user():
    pass


@app.route('/group/create', methods=['POST'])
def create_group():
    pass


@app.route('/user/<username>/pubkey')
def get_pubkey(username):
    pass


@app.route('/user/<username>/new_groups')
def check_new_groups(username):
    pass

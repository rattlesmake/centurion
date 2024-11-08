# ---------------------------
# CENTURION
# [2019] - [2022] Rattlesmake
# All Rights Reserved.
# ---------------------------

# Script for Match_ui.xml

#### GLOBAL FUNCTIONS

def match_quit(args):
    args['match'].Quit()
    args['iframe'].Close()
	
def gamemenu_close(args):
    args['iframe'].Close()
	
def match_save(args):
    args['match'].Save()

def match_load(args):
    args['match'].Load()
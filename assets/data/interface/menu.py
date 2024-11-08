# ---------------------------
# CENTURION
# [2019] - [2022] Rattlesmake
# All Rights Reserved.
# ---------------------------

# Script for Menu.xml

##### MAINMENU #####
def mainmenu_iframe0_btn0_click(args): # game close
    args['engine'].GameClose()    
    
def mainmenu_iframe0_btn1_click(args): # open Match page
    args['menu'].OpenPage('match')

def mainmenu_iframe0_btn2_click(args): # load a game
    args['menu'].OpenPage('load_game')

def mainmenu_iframe0_btn3_click(args): # open Editor
    args['engine'].OpenEnvironment('Editor')
    
def mainmenu_iframe0_btn4_click(args): # open MDS
    print('Tool MDS is opening... Not really, it is needed to be implemented!')
    args['engine'].OpenEnvironment('MDS')

def mainmenu_iframe0_btn5_click(args): # open Options page
    args['menu'].OpenPage('options')

##### MATCH #####

# iframe 0
def match_iframe0_btn0_click(args): # open settings
    args['menu'].OpenPage('match_customizing')

def match_iframe0_btn1_click(args): # start game
    for i in range(8):
        this_iframe = args['iframes']['match.1']
        s = str(i)
        args['settings'].matchPreferences.playersSettings[i].active = this_iframe['checkBox#'+s].checked
        args['settings'].matchPreferences.playersSettings[i].race = this_iframe['comboBox#'+s].selectedText
        args['settings'].matchPreferences.playersSettings[i].name = this_iframe['textInput#'+s].text

    args['engine'].OpenEnvironment('Match')
    
def match_iframe0_btn2_click(args): # close
    args['menu'].OpenPage('mainmenu')

# iframe 1
def match_iframe1_opening(args):
    # new part-----------------

    #todo (from cpp)
    list_of_races = ['egypt', 'rome']
    list_of_difficulties = ['player_no_ai', 'defensive_easy', 'defensive_medium', 'defensive_hard', 'offensive_easy', 'offensive_medium', 'offensive_hard']
    list_of_teams = ['1', '2', '3', '4'] 
    list_of_bonuses = ['Bonus 1', 'Bonus 2', 'Bonus 3'] 
    
    playersSettings = args['settings'].matchPreferences.playersSettings
    for i in range(8):
        playerSettings = playersSettings[i]
        s0 = str(i)
        s8 = str(i+8)
        s16 = str(i+16)
        s24 = str(i+24)
        args['iframe']['checkBox#'+s0].checked = playerSettings.active
        args['iframe']['textInput#'+s0].SetPlaceholder(playerSettings.name, False)        
        args['iframe']['comboBox#'+s0].UpdateOptions(list_of_races, 'r_', playerSettings.race)
        args['iframe']['comboBox#'+s8].UpdateOptions(list_of_difficulties, 'w_', '')
        args['iframe']['comboBox#'+s16].UpdateOptions(list_of_teams, '', '')
        args['iframe']['comboBox#'+s24].UpdateOptions(list_of_bonuses, '', '')

    #-- Update colorPicker #todo
    #for i=0,7 do
	   # local color = GetPlayerById(i + 1):GetColor()
	   # this:SetColorPickerColorById(i, color)
    #end

    # Hide elements for non-active players and set environment #todo
    #for i in range(8):
    #    args['iframe'].HideElementByTagAndId('textInput', i)
    #    args['iframe'].HideElementByTagAndId('colorPicker', i)
    #    args['iframe'].HideElementByTagAndId('comboBox', i)
    #    args['iframe'].HideElementByTagAndId('comboBox', i + 8)
    #    args['iframe'].HideElementByTagAndId('comboBox', i + 16)
    #    args['iframe'].HideElementByTagAndId('comboBox', i + 24)

    args['iframe']['checkBox#0'].checked = True
    # args['iframe']['button#1'].enabled = True #todo temporary
    print('match_iframe1_opening')

def match_iframe1_runtime(args):
    #if (PlayerArray.GetNumberOfPlayers() >= 1) then
	#	Menu.GetIframe(2, 0):EnableButtonById(1)
    #else
	#	Menu.GetIframe(2, 0):DisableButtonById(1)
    #end
    
    # args['menu'].GetIframe('match', 0).EnableButtonById(1) #todo
    x = ''

def match_iframe1_chkbox_util(checkboxID):

    #todo fare in modo che la funzione si adatti in base all'id

    #local nPlayers = PlayerArray.GetNumberOfPlayers()

    #if (this:IsCheckBoxSelectedById(0)) then
	   # nPlayers = nPlayers + 1;
	   # this:ShowElementByTagAndId('textInput', 0)
	   # this:ShowElementByTagAndId('colorPicker', 0)
	   # this:ShowElementByTagAndId('comboBox', 0)
	   # this:ShowElementByTagAndId('comboBox', 8)
	   # this:ShowElementByTagAndId('comboBox', 16)
	   # this:ShowElementByTagAndId('comboBox', 24)
    #else
	   # nPlayers = nPlayers - 1;
	   # this:HideElementByTagAndId('textInput', 0)
	   # this:HideElementByTagAndId('colorPicker', 0)
	   # this:HideElementByTagAndId('comboBox', 0)
	   # this:HideElementByTagAndId('comboBox', 8)
	   # this:HideElementByTagAndId('comboBox', 16)
	   # this:HideElementByTagAndId('comboBox', 24)
    #end

    #PlayerArray.SetNumberOfPlayers(nPlayers)
    print('match_iframe1_chkbox_util; id = ' + str(checkboxID))

def match_iframe1_chkbox0_click(args):
    match_iframe1_chkbox_util(0)

def match_iframe1_chkbox1_click(args):
    match_iframe1_chkbox_util(1)

def match_iframe1_chkbox2_click(args):
    match_iframe1_chkbox_util(2)

def match_iframe1_chkbox3_click(args):
    match_iframe1_chkbox_util(3)

def match_iframe1_chkbox4_click(args):
    match_iframe1_chkbox_util(4)

def match_iframe1_chkbox5_click(args):
    match_iframe1_chkbox_util(5)

def match_iframe1_chkbox6_click(args):
    match_iframe1_chkbox_util(6)

def match_iframe1_chkbox7_click(args):
    match_iframe1_chkbox_util(7)

##### MATCH CUSTOMIZING #####

# iframe 0
def matchcust_iframe0_btn0_click(args): # apply
    
    match_cust_iframe = args['iframes']['match_customizing.3']

    args['settings'].matchPreferences.randomMapSettings.scenarioSize = match_cust_iframe['comboBox#0'].selectedText
    args['settings'].matchPreferences.randomMapSettings.scenarioType = match_cust_iframe['comboBox#1'].selectedText
    
    print('[DEBUG] Selected map type: ' + args['settings'].matchPreferences.randomMapSettings.scenarioType)
    print('[DEBUG] Selected map size: ' + args['settings'].matchPreferences.randomMapSettings.scenarioSize)
    args['menu'].OpenPage('match')

def matchcust_iframe0_btn1_click(args): # cancel
    args['menu'].OpenPage('match')

# Iframe 1
def matchcust_iframe1_opening(args):
    #Scaling all sub-iframes according to the game's resolution
    
    args['iframes']['match_customizing.2'].position = (args['iframes']['match_customizing.1'].position[0] + 20, args['iframes']['match_customizing.1'].position[1] + 40)
    args['iframes']['match_customizing.2'].size = (args['iframes']['match_customizing.1'].size[0] / 2.35, args['iframes']['match_customizing.1'].size[1] - 55)
    
    args['iframes']['match_customizing.3'].position = (args['iframes']['match_customizing.1'].position[0] + 20 + args['iframes']['match_customizing.2'].size[0] + 20, args['iframes']['match_customizing.1'].position[1] + 40)
    args['iframes']['match_customizing.3'].size = (args['iframes']['match_customizing.1'].size[0] / 1.95, 90)
    
    args['iframes']['match_customizing.4'].position = (args['iframes']['match_customizing.1'].position[0] + 20 + args['iframes']['match_customizing.2'].size[0] + 20, args['iframes']['match_customizing.1'].position[1] + 40 + 90 + 20)
    args['iframes']['match_customizing.4'].size = (args['iframes']['match_customizing.1'].size[0] / 1.95, args['iframes']['match_customizing.1'].size[1] - 55 - 90 - 20)

# iframe 3
def matchcust_iframe3_opening(args):
    print('[DEBUG] Map sizes: ' + str(args['engine'].GetScenarioTypes()))
    print('[DEBUG] Map types: ' + str(args['engine'].GetScenarioSizes()))
    args['iframes']['match_customizing.3']['comboBox#0'].UpdateOptions(args['engine'].GetScenarioSizes(), 'w_', args['settings'].matchPreferences.randomMapSettings.scenarioSize)
    args['iframes']['match_customizing.3']['comboBox#1'].UpdateOptions(args['engine'].GetScenarioTypes(), 'w_', args['settings'].matchPreferences.randomMapSettings.scenarioType)

##### LOAD GAME #####

# iframe 1
def loadgame_iframe1_opening(args):
	#Needed to overwrite skin default ones
    args['iframes']['load_game.1']['button#0'].SetImage("common/buttons/button_1")
    args['iframes']['load_game.1']['button#1'].SetImage("common/buttons/button_1")
    args['iframes']['load_game.1']['button#2'].SetImage("common/buttons/button_1")
	
	#Updating elements
    saveFiles = args['engine'].GetListOfSaveFiles()
    args['iframes']['load_game.1']['textList#0'].UpdateOptions(saveFiles, "", "")
    args['iframes']['load_game.1']['text#0'].SetText("Tempo di gioco: " + "N/A", False) #TRANSLATION NEEDED
    args['iframes']['load_game.1']['text#1'].SetText("Difficoltà: " + "N/A", False) #TRANSLATION NEEDED
    args['iframes']['load_game.1']['text#2'].SetText("Numero di giocatori: " + "N/A", False) #TRANSLATION NEEDED

def loadgame_iframe1_txtlst0(args):
    fileName = args['iframes']['load_game.1']['textList#0'].selectedText
	
    #Update viewport elements according to SaveFile
    args['iframes']['load_game.1']['text#0'].SetText("Tempo di gioco: " + args['menu'].GetFileSaveInfo(fileName).time, False) #TRANSLATION NEEDED
    args['iframes']['load_game.1']['text#1'].SetText("Difficoltà: " + args['menu'].GetFileSaveInfo(fileName).difficulty, False) #TRANSLATION NEEDED
    args['iframes']['load_game.1']['text#2'].SetText("Numero di giocatori: " + str(args['menu'].GetFileSaveInfo(fileName).playersNumber), False) #TRANSLATION NEEDED
    args['iframes']['load_game.1']['image#0'].SetImage(fileName)

def loadgame_iframe1_btn0_click(args): # load game button
    args['menu'].Load("quicksave")

def loadgame_iframe1_btn1_click(args): # delete button
    print('[DEBUG] Game deleted! Sorta...')

def loadgame_iframe1_btn2_click(args): # exit button
    args['menu'].OpenPage('mainmenu')

##### OPTIONS #####

# iframe 0
def options_iframe0_btn0_click(args): # apply button
    selectedLan = args['iframes']['options.1']['comboBox#0'].selectedText
    print('[DEBUG] Setting language: ' + selectedLan)
    args['settings'].globalPreferences.language = selectedLan

def options_iframe0_btn1_click(args): # save button
    selectedLan = args['iframes']['options.1']['comboBox#0'].selectedText
    print('[DEBUG] Setting language: ' + selectedLan)
    args['settings'].globalPreferences.language = selectedLan
    args['menu'].OpenPage('mainmenu')

def options_iframe0_btn2_click(args): # exit button
    args['menu'].OpenPage('mainmenu')

# iframe 1
def options_iframe1_opening(args): # opening script
    languages = args['engine'].GetAvailableLanguages()
    print('[DEBUG] Languages: ' + str(languages))
    currentLan = args['settings'].globalPreferences.language
    print('[DEBUG] Current language: ' + currentLan)
    args['iframes']['options.1']['comboBox#0'].UpdateOptions(languages, 'l_', currentLan)




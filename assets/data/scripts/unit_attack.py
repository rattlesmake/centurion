def unit_attack_condition(args):
    return True

    
def unit_attack_body(args):			
    this = args['this']
    target = args['target']
    target_obj = target.object
    this.GoToAttack(target_obj)    
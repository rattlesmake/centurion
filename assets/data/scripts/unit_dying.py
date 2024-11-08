def unit_dying_condition(args):
    this = args['this']
    if (this.IsDead() == False):
        return False
    return True
    
def unit_dying_body(args):
    this = args['this']
    print("unit_dying body")
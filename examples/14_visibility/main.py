from codac import *


class MySep(Sep):

  def __init__(self, y_):
    Sep.__init__(self, y_.size())
    self.y = y_ # r,theta

  def separate(self, x):
    ctc_polar = CtcPolar()
    x_aug = IntervalVector([x[0],x[1],self.y[0],self.y[1]])

    ctc_polar.contract(x_aug)

    x_out = x_aug.subvector(0,1)

    test = IntervalVector([[0,oo],[-PI,PI]]).diff(self.y)

    x_not_in = IntervalVector.empty(2)

    for not_cont in test:
      x_aug_notin = IntervalVector([x[0],x[1],not_cont[0],not_cont[1]])
      ctc_polar.contract(x_aug_notin)
      x_not_in |= x_aug_notin.subvector(0,1)

    return [x_not_in,x_out]
  
c = Vector([0,0])
y = IntervalVector([[1,5],[0,PI/2]])

x1 = IntervalVector([[1.2,1.8],[-0.4,0.4]])

sep = MySep(y)

a = IntervalVector([2,3])
a.inflate(0.1)
s = Segment([[1,1],[4,4]],[[3,3],[2,2]])

DefaultFigure.draw_box(a)
# DefaultFigure.pave([[-1,6],[-1,6]],SepVisible(a, s), 0.1)
# DefaultFigure.pave([[-1,6],[-1,6]], sep, 0.1)
DefaultFigure.pave([[-1,6],[-1,6]],SepVisible(a, s)&sep, 0.1)


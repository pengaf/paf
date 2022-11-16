
idlcpp.tutorial.Test.Run()

pt = idlcpp.tutorial.Point();
pt.x = 1;
pt.y = 2;

x = idlcpp.float._NewUniquePtr_();
pt:getX(x);

print(x[0]);
print(pt.x);
print(pt.y);
print(pt:test());
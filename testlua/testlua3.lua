
hw = idlcpp.tutorial.Test.Run()
print(hw);

min, max = idlcpp.tutorial.Test.Sort2(5,3);
print(min, max);

pt = idlcpp.tutorial.Point();
pt.x = 1;
pt.y = 2;

x = idlcpp.float._New_();
pt:getX(x);


--print(x[0]);
--print(pt.x);
--print(pt.y);
--print(pt:test());
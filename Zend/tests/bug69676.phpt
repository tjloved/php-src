--TEST--
Bug #69676: Resolution of self::FOO in class constants not correct
--FILE--
<?php

class A
{
    const myConst = "const in A";
    const myDynConst = self::myConst;
}
class B extends A
{
    const myConst = "const in B";
}
function fn280774865()
{
    var_dump(B::myDynConst);
    var_dump(A::myDynConst);
}
fn280774865();
--EXPECT--
string(10) "const in A"
string(10) "const in A"

--TEST--
Bug #62343 (Show class_alias In get_declared_classes())
--FILE--
<?php

class a
{
}
function fn748159280()
{
    class_alias("a", "b");
    $c = get_declared_classes();
    var_dump(end($c));
    var_dump(prev($c));
}
fn748159280();
--EXPECT--
string(1) "b"
string(1) "a"

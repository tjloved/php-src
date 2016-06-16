--TEST--
Leak in QM_ASSIGN when unwrapping references (rc=1)
--FILE--
<?php

function &ref()
{
    $str = "str";
    $str .= "str";
    return $str;
}
function fn276849021()
{
    var_dump(true ? ref() : ref());
    var_dump(ref() ?: ref());
    var_dump(ref() ?? ref());
}
fn276849021();
--EXPECT--
string(6) "strstr"
string(6) "strstr"
string(6) "strstr"

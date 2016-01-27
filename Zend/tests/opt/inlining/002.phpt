--TEST--
Inlining 002
--FILE--
<?php

function returnVal() {
    return 42;
}

function &returnRef2() {
    $a = 42;
    return $a;
}

function &returnRef() {
    $x = returnRef2();
    return returnVal();
}

function assignRef() {
    $x = returnRef2();
    $var =& returnVal();
}

returnRef();
assignRef();

?>
--EXPECTF--
Notice: Only variable references should be returned by reference in %s on line %d

Notice: Only variables should be assigned by reference in %s on line %d

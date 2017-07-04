--TEST--
Closure 012: Undefined lexical variables
--FILE--
<?php

function fn886814209()
{
    $lambda = function () use($i) {
        return ++$i;
    };
    $lambda();
    $lambda();
    var_dump($i);
    $lambda = function () use(&$i) {
        return ++$i;
    };
    $lambda();
    $lambda();
    var_dump($i);
}
fn886814209();
--EXPECTF--
Notice: Undefined variable: i in %sclosure_012.php on line %d

Notice: Undefined variable: i in %sclosure_012.php on line %d
NULL
int(2)


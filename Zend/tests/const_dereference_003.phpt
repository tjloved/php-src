--TEST--
Const array deference
--FILE--
<?php

function fn1758785765()
{
    error_reporting(E_ALL);
    var_dump([1, 2, 3, 4][3]);
    var_dump([1, 2, 3, 4]['foo']);
    var_dump([array(1, 2, 3), [4, 5, 6]][1][2]);
    foreach (array([1, 2, 3])[0] as $var) {
        echo $var;
    }
}
fn1758785765();
--EXPECTF--
int(4)

Notice: Undefined index: foo in %sconst_dereference_003.php on line %d
NULL
int(6)
123

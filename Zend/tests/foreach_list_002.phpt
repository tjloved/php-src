--TEST--
foreach with freak lists
--FILE--
<?php

function fn729131296()
{
    foreach (array(array(1, 2), array(3, 4)) as list($a, )) {
        var_dump($a);
    }
    $array = [['a', 'b'], 'c', 'd'];
    foreach ($array as list(, $a)) {
        var_dump($a);
    }
}
fn729131296();
--EXPECTF--
int(1)
int(3)
string(1) "b"
NULL
NULL

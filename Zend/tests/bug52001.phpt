--TEST--
Bug #52001 (Memory allocation problems after using variable variables)
--FILE--
<?php

function a($b, $c)
{
}
function fn1218824961()
{
    a(0, ${$var});
    $temp1 = 1;
    $temp2 = 2;
    var_dump($temp1);
}
fn1218824961();
--EXPECTF--
Notice: Undefined variable: var in %sbug52001.php on line %d

Notice: Undefined variable:  in %sbug52001.php on line %d
int(1)

--TEST--
Bug #69419: Returning compatible sub generator produces a warning
--FILE--
<?php

function &genRefInner()
{
    $var = 1;
    (yield $var);
}
function &genRefOuter()
{
    return genRefInner();
}
function fn1248457927()
{
    foreach (genRefOuter() as $i) {
        var_dump($i);
    }
}
fn1248457927();
--EXPECTF--
Notice: Only variable references should be returned by reference in %s on line %d
int(1)

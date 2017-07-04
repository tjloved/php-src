--TEST--
Explicitly nullable int type
--FILE--
<?php

function _int_(?int $v) : ?int
{
    return $v;
}
function fn641502014()
{
    var_dump(_int_(null));
    var_dump(_int_(1));
}
fn641502014();
--EXPECT--
NULL
int(1)


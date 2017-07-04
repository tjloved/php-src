--TEST--
Explicitly nullable float type
--FILE--
<?php

function _float_(?float $v) : ?float
{
    return $v;
}
function fn1273751173()
{
    var_dump(_float_(null));
    var_dump(_float_(1.3));
}
fn1273751173();
--EXPECT--
NULL
float(1.3)


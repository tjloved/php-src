--TEST--
Explicitly nullable array type
--FILE--
<?php

function _array_(?array $v) : ?array
{
    return $v;
}
function fn1497179682()
{
    var_dump(_array_(null));
    var_dump(_array_([]));
}
fn1497179682();
--EXPECT--
NULL
array(0) {
}


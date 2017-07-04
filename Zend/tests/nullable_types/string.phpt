--TEST--
Explicitly nullable string type
--FILE--
<?php

function _string_(?string $v) : ?string
{
    return $v;
}
function fn1391561810()
{
    var_dump(_string_(null));
    var_dump(_string_("php"));
}
fn1391561810();
--EXPECT--
NULL
string(3) "php"


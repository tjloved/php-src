--TEST--
Bug #70332 (Wrong behavior while returning reference on object)
--FILE--
<?php

function &test($arg)
{
    return $arg;
}
function fn602589493()
{
    $arg = new Stdclass();
    $arg->name = array();
    test($arg)->name[1] = "xxxx";
    print_r($arg);
}
fn602589493();
--EXPECT--
stdClass Object
(
    [name] => Array
        (
            [1] => xxxx
        )

)

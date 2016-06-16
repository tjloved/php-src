--TEST--
output buffering - ob_list_handlers
--FILE--
<?php

function fn1798427130()
{
    print_r(ob_list_handlers());
    ob_start();
    print_r(ob_list_handlers());
    ob_start();
    print_r(ob_list_handlers());
    ob_end_flush();
    print_r(ob_list_handlers());
    ob_end_flush();
    print_r(ob_list_handlers());
}
fn1798427130();
--EXPECT--
Array
(
)
Array
(
    [0] => default output handler
)
Array
(
    [0] => default output handler
    [1] => default output handler
)
Array
(
    [0] => default output handler
)
Array
(
)

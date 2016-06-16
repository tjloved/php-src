--TEST--
Instantiate stdClass
--FILE--
<?php

function fn674748588()
{
    $obj = new stdClass();
    echo get_class($obj) . "\n";
    echo "Done\n";
}
fn674748588();
--EXPECTF--
stdClass
Done

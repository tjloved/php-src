--TEST--
Bug #69893: Strict comparison between integer and empty string keys crashes
--FILE--
<?php

function fn684210205()
{
    var_dump([0 => 0] === ["" => 0]);
}
fn684210205();
--EXPECT--
bool(false)

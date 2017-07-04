--TEST--
Bug #69893: Strict comparison between integer and empty string keys crashes
--FILE--
<?php

function fn96364912()
{
    var_dump([0 => 0] === ["" => 0]);
}
fn96364912();
--EXPECT--
bool(false)

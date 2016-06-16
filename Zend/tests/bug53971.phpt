--TEST--
Bug #53971 (isset() and empty() produce apparently spurious runtime error)
--FILE--
<?php

function fn311814611()
{
    $s = "";
    var_dump(isset($s[0][0]));
}
fn311814611();
--EXPECT--
bool(false)



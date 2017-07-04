--TEST--
Bug #53971 (isset() and empty() produce apparently spurious runtime error)
--FILE--
<?php

function fn1291430715()
{
    $s = "";
    var_dump(isset($s[0][0]));
}
fn1291430715();
--EXPECT--
bool(false)



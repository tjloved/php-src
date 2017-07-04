--TEST--
Nowdocs CAN be used as static scalars.
--FILE--
<?php

class e
{
    const E = <<<'THISMUSTNOTERROR'
If you DON'T see this, something's wrong.
THISMUSTNOTERROR;
}
function fn895463508()
{
    require_once 'nowdoc.inc';
    print e::E . "\n";
}
fn895463508();
--EXPECTF--
If you DON'T see this, something's wrong.

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
function fn879469105()
{
    require_once 'nowdoc.inc';
    print e::E . "\n";
}
fn879469105();
--EXPECTF--
If you DON'T see this, something's wrong.

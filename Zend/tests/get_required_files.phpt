--TEST--
Check if get_required_files works
--CREDITS--
Sebastian Schürmann
sschuermann@chip.de
Testfest 2009 Munich
--FILE--
<?php

function fn1311639061()
{
    $files = get_required_files();
    var_dump($files);
}
fn1311639061();
--EXPECTF--
array(1) {
  [0]=>
  %string|unicode%(%d)%s
}

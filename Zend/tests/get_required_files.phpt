--TEST--
Check if get_required_files works
--CREDITS--
Sebastian Schürmann
sschuermann@chip.de
Testfest 2009 Munich
--FILE--
<?php

function fn2121566873()
{
    $files = get_required_files();
    var_dump($files);
}
fn2121566873();
--EXPECTF--
array(1) {
  [0]=>
  %string|unicode%(%d)%s
}

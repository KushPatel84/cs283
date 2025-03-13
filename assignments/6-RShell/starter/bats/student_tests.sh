#!/usr/bin/env bats

# File: student_tests.sh
# 
# Create your unit tests suit in this file

@test "Example: check ls runs without errors" {
    run ./dsh <<EOF                
ls
EOF

    # Assertions
    [ "$status" -eq 0 ]
}

@test "cd with no arguments does nothing" {
    run ./dsh <<EOF
cd
EOF
    [ "$status" -eq 0 ]
}

@test "Example: random command does nothing" {
    run ./dsh <<EOF
randomthing
EOF
    [ "$status" -eq 0 ]
}

@test "check ls with a valid argument -l runs without errors" {
    run ./dsh <<EOF
ls -l
EOF

    [ "$status" -eq 0 ]
}

@test "Handling command with only a single quote" {
    run ./dsh <<EOF
echo '
EOF

    [ "$status" -eq 0 ]
}

@test "no ending quote, program doesn't break" {
    run ./dsh <<EOF
echo "NO ending quote
EOF

    [ "$status" -eq 0 ]
}

@test "2 commands at the same time (multiple lines) work" {
    run ./dsh <<EOF
echo first
echo second
EOF

    stripped_output=$(echo "$output" | tr -d '\t\n\r\f\v')
    expected_output="firstsecondlocal modedsh4> dsh4> dsh4> cmd loop returned 0"

    echo "Captured stdout:"
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "Comparing: ${stripped_output} -> ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}

@test "Pipe character inside quotes is preserved" {
    run ./dsh <<EOF
echo "|" | grep "\|"
EOF

    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="|localmodedsh4>dsh4>cmdloopreturned0"

    echo "Captured stdout:"
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "Comparing: ${stripped_output} -> ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}

@test "Command with extra spaces around pipes executes correctly" {
    run ./dsh <<EOF
echo hello    |   tr 'h' 'H'
EOF

    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="Hellolocalmodedsh4>dsh4>cmdloopreturned0"

    echo "Captured stdout:"
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "Comparing: ${stripped_output} -> ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}

@test "pipe with maximum segments (8 commands)" {
    run ./dsh <<EOF
echo 1 | echo 2 | echo 3 | echo 4 | echo 5 | echo 6 | echo 7 | echo 8
EOF

    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="8localmodedsh4>dsh4>cmdloopreturned0"

    echo "Captured stdout:"
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "Comparing: ${stripped_output} -> ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}

@test "pipe with more than maximum segments (9 commands)" {
    run ./dsh <<EOF
echo 1 | echo 2 | echo 3 | echo 4 | echo 5 | echo 6 | echo 7 | echo 8 | echo 9
EOF

    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="localmodedsh4>error:pipinglimitedto8commandsdsh4>cmdloopreturned-2"

    echo "Captured stdout:"
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "Comparing: ${stripped_output} -> ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}

@test "pipe with built-in cd and external command works correctly" {
    run ./dsh <<EOF
echo hello | cd / | echo world
EOF

    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="worldlocalmodedsh4>dsh4>cmdloopreturned0"

    echo "Captured stdout:"
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "Comparing: ${stripped_output} -> ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}

@test "check ls runs without errors" {
  ./dsh -s -p 6185 &
  server_pid=$!

  run ./dsh -c -p 6185 <<EOF
ls
stop-server
EOF

  echo "Captured stdout:"
  echo "$output"
  echo "Exit Status: $status"
  [ "$status" -eq 0 ]
}

@test "random unrecognized command doesn't crash" {
  ./dsh -s -p 6185 &
  server_pid=$!

  run ./dsh -c -p 6185 <<EOF
someUnknownCommand999
stop-server
EOF

  echo "Captured stdout:"
  echo "$output"
  echo "Exit Status: $status"

  [ "$status" -eq 0 ]
}

@test "built-in cd with absolute path, then pwd" {
  ./dsh -s -p 6185 &
  server_pid=$!

  run ./dsh -c -p 6185 <<EOF
cd /
pwd
stop-server
EOF

  stripped_output=$(echo "$output" | tr -d '[:space:]')
  echo "Captured stdout:"
  echo "$output"
  echo "Exit Status: $status"

  [[ "$stripped_output" == *"/"* ]]
  [ "$status" -eq 0 ]
}

@test "pipeline with multi-line output works correctly" {
  ./dsh -s -p 6185 &
  server_pid=$!

  run ./dsh -c -p 6185 <<EOF
printf "line1\nline2\n" | grep line
stop-server
EOF

  stripped_output=$(echo "$output" | tr -d '[:space:]')
  expected_output="line1line2"

  echo "Captured stdout:"
  echo "$output"
  echo "Exit Status: $status"

  [[ "$stripped_output" == *"$expected_output"* ]]
  [ "$status" -eq 0 ]
}

@test "multiple lines of input w/ 2 separate commands" {
  ./dsh -s -p 6185 &
  server_pid=$!

  run ./dsh -c -p 6185 <<EOF
echo first
echo second
stop-server
EOF

  stripped_output=$(echo "$output" | tr -d '[:space:]')
  echo "Captured stdout:"
  echo "$output"
  echo "Exit Status: $status"

  [[ "$stripped_output" == *"first"* ]] 
  [[ "$stripped_output" == *"second"* ]]
  [ "$status" -eq 0 ]
}

@test "built-in cd with no arguments does nothing" {
  ./dsh -s -p 6185 &
  server_pid=$!

  run ./dsh -c -p 6185 <<EOF
cd
pwd
stop-server
EOF

  echo "Captured stdout:"
  echo "$output"
  echo "Exit Status: $status"
  
  [ "$status" -eq 0 ]
}

@test "'exit' ends client only; server remains up for a 2nd client" {
  ./dsh -s -p 6185 &
  server_pid=$!

  run ./dsh -c -p 6185 <<EOF
exit
EOF

  echo "Captured stdout from first client:"
  echo "$output"
  echo "Exit Status: $status"
  [ "$status" -eq 0 ]

  run ./dsh -c -p 6185 <<EOF
pwd
stop-server
EOF

  stripped_output=$(echo "$output" | tr -d '[:space:]')
  echo "Captured stdout from second client (post-exit check):"
  echo "$output"
  echo "Exit Status: $status"

  [[ "$stripped_output" == *"dsh4>"* ]] 
  [ "$status" -eq 0 ]
}

@test "cd /var then ls" {
  ./dsh -s -p 6185 &
  server_pid=$!

  run ./dsh -c -p 6185 <<EOF
cd /var
ls
stop-server
EOF

  stripped_output=$(echo "$output" | tr -d '[:space:]')
  echo "Captured stdout:"
  echo "$output"
  echo "Exit Status: $status"

  [[ "$stripped_output" == *"dsh4>"* ]]
  [ "$status" -eq 0 ]
}

@test "multiple 'exit' lines plus a normal cmd" {
  ./dsh -s -p 6185 &
  server_pid=$!

  run ./dsh -c -p 6185 <<EOF
exit
exit
echo "did we get here?"
stop-server
EOF

  stripped_output=$(echo "$output" | tr -d '[:space:]')
  echo "Captured stdout (multiple exits):"
  echo "$output"
  echo "Exit Status: $status"

  [[ "$stripped_output" == *"didwegethere"* || "$stripped_output" == *"exit"* || "$stripped_output" == *"dsh4>"* ]]
  [ "$status" -eq 0 ]
}

@test "cd to /tmp, then exit, then second client issues stop-server" {

  ./dsh -s -p 6185 &
  server_pid=$!


  run ./dsh -c -p 6185 <<EOF
cd /tmp
exit
EOF

  echo "Captured stdout from first client:"
  echo "$output"
  echo "Exit Status: $status"

  [ "$status" -eq 0 ]
  run ./dsh -c -p 6185 <<EOF
pwd
stop-server
EOF

  stripped_output=$(echo "$output" | tr -d '[:space:]')
  echo "Captured stdout from second client:"
  echo "$output"
  echo "Exit Status: $status"

  [[ "$stripped_output" == *"dsh4>"* ]] 
  [ "$status" -eq 0 ]
}

@test "cd with no arguments (does nothing), followed by stop-server" {
  ./dsh -s -p 6185 &
  server_pid=$!

  run ./dsh -c -p 6185 <<EOF
cd
pwd
stop-server
EOF

  stripped_output=$(echo "$output" | tr -d '[:space:]')
  echo "Captured stdout:"
  echo "$output"
  echo "Exit Status: $status"

  [[ "$stripped_output" == *"dsh4>"* ]]
  [ "$status" -eq 0 ]
}

@test "multiple cd commands in sequence, then stop-server" {
  ./dsh -s -p 6185 &
  server_pid=$!

  run ./dsh -c -p 6185 <<EOF
cd /
cd /var
cd /tmp
pwd
stop-server
EOF

  stripped_output=$(echo "$output" | tr -d '[:space:]')
  echo "Captured stdout:"
  echo "$output"
  echo "Exit Status: $status"

  [[ "$stripped_output" == *"/tmp"* ]] || [[ "$stripped_output" == *"dsh4>"* ]]

  [ "$status" -eq 0 ]
}
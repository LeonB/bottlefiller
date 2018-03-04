to_entries | map(
    .key as $key
    | .value
    | map(select(.time > 100))
    | length as $length
    | (([.[].time] | add) / $length) as $average_time
    | ((([.[].scale_update.weight] | add) - ([.[].bottle_weight] | add)) / $length) as $average_weight_filled
    | (([.[].loops] | add) / $length) as $average_loops
    | {
        ($key): {
            length: $length,
            average_weight_deviation: (([.[].full_weight_deviation] | add) / $length),
            max_weight_deviation: ([.[].full_weight_deviation] | max),
            average_time: $average_time,
            average_weight_filled: $average_weight_filled,
            average_fill_rate_per_second: ($average_weight_filled / ($average_time/1000)),
            average_fill_rate_per_loop: ($average_weight_filled / $average_loops)
        }
    }
) | add

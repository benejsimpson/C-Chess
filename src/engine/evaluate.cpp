#include "engine/evaluate.hpp"

#include <cmath>

using namespace std;

namespace
{
    const string FILE_PATH = "src/tests/output/Evals.csv";

    struct CsvRow
    {
        string fen;
        string stockfish_eval;
    };

    vector<string> parse_csv_row(const string& line)
    {
        vector<string> fields;
        string field;
        bool in_quotes = false;

        for (size_t i = 0; i < line.size(); ++i)
        {
            const char ch = line[i];

            if (ch == '"')
            {
                if (in_quotes && i + 1 < line.size() && line[i + 1] == '"')
                {
                    field += '"';
                    ++i;
                }
                else
                {
                    in_quotes = !in_quotes;
                }
            }
            else if (ch == ',' && !in_quotes)
            {
                fields.push_back(field);
                field.clear();
            }
            else
            {
                field += ch;
            }
        }

        fields.push_back(field);
        return fields;
    }

    string escape_csv_field(const string& field)
    {
        if (field.find_first_of("\",") == string::npos)
        {
            return field;
        }

        string escaped = "\"";
        for (const char ch : field)
        {
            if (ch == '"')
            {
                escaped += "\"\"";
            }
            else
            {
                escaped += ch;
            }
        }
        escaped += "\"";
        return escaped;
    }

    vector<CsvRow> read_rows_from_csv(const string& file_path)
    {
        ifstream file(file_path);
        vector<CsvRow> rows;

        if (!file.is_open())
        {
            return rows;
        }

        string line;
        bool first_row = true;

        while (getline(file, line))
        {
            if (line.empty())
            {
                continue;
            }

            const auto fields = parse_csv_row(line);
            if (fields.empty())
            {
                continue;
            }

            if (first_row)
            {
                first_row = false;

                if (!fields.empty() && (fields[0] == "FEN" || fields[0] == "fen"))
                {
                    continue;
                }
            }

            if (!fields.empty() && !fields[0].empty())
            {
                rows.push_back({
                    fields[0],
                    fields.size() > 1 ? fields[1] : ""
                });
            }
        }

        return rows;
    }
}

vector<EvalResult> evaluate_positions_from_csv(
    const string& file_path,
    const string& version)
{
    vector<EvalResult> results;
    const auto rows = read_rows_from_csv(file_path);

    for (const CsvRow& row : rows)
    {
        Board board;
        load_fen(board, row.fen);

        const int eval_score = evaluate(board);

        results.push_back({
            row.fen,
            row.stockfish_eval,
            eval_score,
            abs(eval_score)
        });
    }

    return results;
}

bool write_eval_results_to_csv(
    const vector<EvalResult>& results,
    const string& file_path,
    const string& version)
{
    ofstream file(file_path);

    if (!file.is_open())
    {
        return false;
    }

    file << "FEN,Stockfish," << escape_csv_field(version) << ",AbsoluteEval\n";

    for (const EvalResult& result : results)
    {
        file << escape_csv_field(result.fen) << ",";
        file << escape_csv_field(result.stockfish_eval) << ",";
        file << result.eval << ",";
        file << result.absolute_eval << "\n";
    }

    return true;
}

bool evaluate_csv_positions(const string& version)
{
    const vector<EvalResult> results = evaluate_positions_from_csv(FILE_PATH, version);
    return write_eval_results_to_csv(results, FILE_PATH, version);
}

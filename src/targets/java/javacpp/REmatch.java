package javacpp;

import org.bytedeco.javacpp.*;
import org.bytedeco.javacpp.annotation.*;


public class REmatch {

    // static { System.loadLibrary("jniREmatch"); }
    static { JNILoader.load("jniREmatch"); }

    public static void main(String[] args) {
        try {
            //////////////////////////////////////////
            ////////////// QUERY EXAMPLE /////////////
            //////////////////////////////////////////

            System.out.println("QUERY EXAMPLE\n");

            String pattern = "!x{aaa}";
            Flags flags = Flags.NONE();
            int maxMempoolDuplications = 8;
            int maxDeterministicStates = 2000;

            Query query = new Query(pattern, flags.getVal(), maxMempoolDuplications, maxDeterministicStates);

            String document = "aaaab";

            System.out.println("Patrón de búsqueda: " + pattern);
            System.out.println("Documento a buscar: " + document + "\n");

            boolean isValid = query.check(document);
            System.out.println("QUERY::CHECK()");
            System.out.println("—— Documento es válido: " + isValid + "\n");

            System.out.println("\nQUERY::VARIABLES()");
            StringVector vars = query.variables();
            System.out.println("——— Variables encontradas en el match (desde Query): " + vars.size());
            for (long i = 0; i < vars.size(); i++) {
                System.out.println("——— Var "+i+": " + vars.at(i));
            }

            System.out.println("\nQUERY::FINDONE()");
            Match result = query.findone(document);

            int variableIdFindone = 0;
            System.out.println("—— MATCH::EMPTY()");
            System.out.println("————— Match está vacío: " + result.empty());
            System.out.println("—— MATCH::TO_STRING()");
            System.out.println("————— String del match: " + result.to_string());
            System.out.println("—— MATCH::START()");
            System.out.println("————— Span de inicio por id: "+ result.start(variableIdFindone));
            System.out.println("————— Span de inicio por variable: "+ result.start("x"));
            System.out.println("—— MATCH::END()");
            System.out.println("————— Span de fin por id: "+ result.end(variableIdFindone));
            System.out.println("————— Span de fin por variable: "+ result.end("x"));
            System.out.println("—— MATCH::GROUP()");
            System.out.println("————— Grupo por id: " + result.group(variableIdFindone));
            System.out.println("————— Grupo por variable: " + result.group("x"));
            System.out.println("—— MATCH::SPAN()");
            System.out.println("————— Span por id (first, second): " + result.span(0).first() + ", " + result.span(0).second());
            System.out.println("————— Span por variable (first, second): " + result.span("x").first() + ", " + result.span("x").second());
            System.out.println("—— MATCH::GROUPDICT()");
            System.out.println("————— Diccionario de grupos por variable (first, second): " + result.groupdict().at("x").first() + ", " + result.groupdict().at("x").second());
            System.out.println("—— MATCH::VARIABLES()");
            StringVector vars_match = result.variables();
            System.out.println("————— Variables encontradas en el match (desde Match): " + vars_match.size());
            for (long i = 0; i < vars_match.size(); i++) {
                System.out.println("————— Var "+i+": " + vars_match.at(i));
            }

            System.out.println("\nQUERY::FINDMANY()");
            int findNumber = 3;
            MatchVector matches = query.findmany(document, findNumber);
            System.out.println("—— Matches encontradas para búsqueda de " + findNumber + ": " + matches.size());
            for (int i = 0; i < matches.size(); i++) {
                Match match = matches.at(i);
                System.out.println("————— " + i + ": " + match.to_string());
            }

            System.out.println("\nQUERY::FINDALL()");
            MatchVector allMatches = query.findall(document);
            System.out.println("Matches encontradas: " + allMatches.size());
            for (int i = 0; i < allMatches.size(); i++) {
                Match match = allMatches.at(i);
                System.out.println("————— " + i + ": " + match.to_string());
            }

            System.out.println("\nQUERY::FINDITER()");
            MatchGenerator matchGenerator = query.finditer(document);
            MatchGeneratorIterator it = matchGenerator.begin();
            System.out.println("Iterando matches encontrados...");
            int k = 0;
            while (!it.operatorEquals(matchGenerator.end())) {
                Match match = it.operator_star();
                System.out.println("————— " + k + ": " + match.to_string());
                it.operator_increment();
                k++;
            }

            System.out.println("\n");






            //////////////////////////////////////////
            ///////////// MULTIQUERY EXAMPLE /////////
            //////////////////////////////////////////

            System.out.println("\n\n\n\n\n\n\n\n");
            System.out.println("MULTIQUERY EXAMPLE\n");

            String patternMulti = "@(!extrasubdomain{\\w+}\\.)?!subdomains{\\w+}\\.!subdomains{\\w+}(\n|$)";
            Flags flagsMulti = Flags.NONE();
            int maxMempoolDuplicationsMulti = 8;
            int maxDeterministicStatesMulti = 2000;

            MultiQuery multiQuery = new MultiQuery(patternMulti, flagsMulti.getVal(), maxMempoolDuplicationsMulti, maxDeterministicStatesMulti);

            String documentMulti = "cperez@gmail.com\n" +
                                "soto@uc.cl\n" +
                                "sdelcampo@gmail.com\n" +
                                "lpalacios@gmeil.com\n" +
                                "pvergara@ing.uc.cl\n" +
                                "ndelafuente@ing.puc.cl\n" +
                                "ldelgado@gmsil.com\n" +
                                "tnovoa@mail.uc.cl\n" +
                                "nnarea@myucmail.uc.cl\n" +
                                "rramirez@gmail.com\n" +
                                "juansoto@uc.cl";

            System.out.println("Patrón de búsqueda: " + patternMulti);
            System.out.println("Documento a buscar: " + documentMulti + "\n");

            System.out.println("MULTIQUERY::CHECK()");
            boolean isValidMulti = multiQuery.check(document);
            System.out.println("—— Documento es válido: " + isValidMulti + "\n");

            System.out.println("MULTIQUERY::VARIABLES()");
            StringVector varsMulti = multiQuery.variables();
            System.out.println("—— Variables encontradas en el match (desde MultiQuery): " + varsMulti.size());
            for (long i = 0; i < varsMulti.size(); i++) {
                System.out.println("—— Var "+i+": " + varsMulti.at(i));
            }


            System.out.println("\nMULTIQUERY::FINDONE()");
            MultiMatch multiMatch = multiQuery.findone(documentMulti);

            int variableId = 1;
            System.out.println("—— MULTIMATCH::EMPTY()");
            System.out.println("————— MultiMatch está vacío: " + multiMatch.empty());
            System.out.println("—— MULTIMATCH::TO_STRING()");
            System.out.println("———— String del multiMatch: " + multiMatch.to_string());
            System.out.println("—— MULTIMATCH::SPANS()");
            long size = multiMatch.spans(variableId).size();
            System.out.println("————— Spans encontrados: " + multiMatch.spans(variableId).size());
            for (long i = 0; i < size; i++) {
                Span span = multiMatch.spans(variableId).at(i); // Obtener cada Span
                long first = span.first();
                long second = span.second();
                System.out.println("————— Span " + i + ": (" + first + ", " + second + ")");
            }
            System.out.println("—— MULTIMATCH::VARIABLES()");
            StringVector variables = multiMatch.variables();
            System.out.println("————— Variables encontradas en el multiMatch (desde MultiMatch): " + variables.size());
                for (long i = 0; i < variables.size(); i++) {
                    System.out.println("————— Var "+i+": " + variables.at(i));
                }
            System.out.println("—— MULTIMATCH::GROUPS()");
            StringVector groupResults = multiMatch.groups(variableId);
            StringVector groupResults2 = multiMatch.groups("subdomains");
            System.out.println("————— Grupos por variable_id " + "'"+variableId+"'" + ": " + groupResults);
            System.out.println("————— Grupos por variable_name 'subdomains': " + groupResults2);
            System.out.println("—— MULTIMATCH::SUBMATCH()");
            long start = multiMatch.spans(variableId).at(0).first();
            long end = multiMatch.spans(variableId).at(0).second();
            Span spanSubmatch = new Span(start, end);
            MultiMatch submatch = multiMatch.submatch(spanSubmatch); // Obtener el submatch para el intervalo especificado
            System.out.println("————— Submatch encontrado para la variable "+"'"+variableId+"':" + submatch.to_string());
            StringVector variablesSubmatch = submatch.groups(variableId);
            System.out.println("————— Variables encontradas en el submatch: " + variablesSubmatch.size());
            for (long i = 0; i < variablesSubmatch.size(); i++) {
                System.out.println("————— Variable " + i + ": " + variablesSubmatch.at(i));
            }
            System.out.println("—— MULTIMATCH::OPERATOREQUALS()");
            String document2 = "xyz.mail@otherdomain.com\n";
            String document3 = "cperez@gmail.com\n";
            String test_document1 = "cperez@gmail.com\n";
            MultiMatch match2 = multiQuery.findone(document2);
            MultiMatch match3 = multiQuery.findone(document3);
            System.out.println("————— match1 == match2: " + multiMatch.operatorEquals(match2)); // Esperado: false
            System.out.println("————— match1 == match3: " + multiMatch.operatorEquals(match3)); // Esperado: true
            System.out.println("————— match2 == match3: " + match2.operatorEquals(match3)); // Esperado: false            
            System.out.println("————— String de Match1: " + multiMatch.to_string());
            System.out.println("————— String de Match3: " + match3.to_string());   // por qué su operatorEquals es false?


            System.out.println("\nMULTIQUERY::FINDMANY()");
            int findNumberMulti = 3;
            MultiMatchVector matches2 = multiQuery.findmany(documentMulti, findNumberMulti);
            System.out.println("—— Matches encontradas para búsqueda de " + findNumberMulti + ": " + matches2.size());
            for (int i = 0; i < matches2.size(); i++) {
                MultiMatch match = matches2.at(i);
                System.out.println("————— " + i + ": " + match.to_string());
            }


            System.out.println("\nMULTIQUERY::FINDALL()");
            MultiMatchVector allMatchesMulti = multiQuery.findall(documentMulti);
            System.out.println("MultiMatches encontradas: " + allMatchesMulti.size());
            for (int i = 0; i < allMatchesMulti.size(); i++) {
                MultiMatch match = allMatchesMulti.at(i);
                System.out.println("————— " + i + ": " + match.to_string());
            }


            System.out.println("\nMULTIQUERY::FINDITER()");
            MultiMatchGenerator multiMatchGenerator = multiQuery.finditer(documentMulti);
            MultiMatchGeneratorIterator iter = multiMatchGenerator.begin();
            System.out.println("Iterando multiMatches encontrados...");
            while (!iter.operatorEquals(multiMatchGenerator.end())) {
                MultiMatch multiMatchIter = iter.operator_star(); // Accede al objeto MultiMatch
                System.out.println(multiMatchIter.to_string()); 
                iter.operator_increment();
            }

            System.out.println("\n");



        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
